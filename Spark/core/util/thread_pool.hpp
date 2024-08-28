#ifndef SPARK_THREAD_POOL_HPP
#define SPARK_THREAD_POOL_HPP

#include <core/pch.hpp>
#include <core/util/log.hpp>

namespace Spark
{
    enum class TaskPriority
    {
        CRITICAL,
        VERY_HIGH,
        HIGH,
        NORMAL,
        LOW,
        VERY_LOW,
        BACKGROUND
    };

    struct ThreadControlBlock
    {
        std::thread::id thread_id;
        std::atomic<bool> is_registered_for_sync{ false };
        std::atomic<bool> has_reached_sync_point{ false };
    };

    class ThreadPool
    {
    public:
        ThreadPool(u32 num_threads = std::thread::hardware_concurrency())
            : m_stop(false), m_active_tasks(0), m_sync_tasks(0)
        {
            Initialize(num_threads);
        }

        ~ThreadPool()
        {
            Shutdown();
        }

        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;

        template <class F, class... Args>
        std::future<typename std::invoke_result<F, Args...>::type> Enqueue(TaskPriority priority, bool synchronize, F&& f, Args&&... args)
        {
            using return_type = typename std::invoke_result<F, Args...>::type;

            auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...));

            std::future<return_type> res = task->get_future();
            {
                std::unique_lock<std::mutex> lock(m_queue_mutex);

                if (m_stop)
                {
                    LOG_ERROR("[THREAD POOL]: Enqueue() called after shutdown");
                    assert(false);
                }

                u32 queue_index = SelectQueue();
                m_tasks_queues[queue_index].emplace_back(priority, [task, synchronize, this]() {
                    try {
                        (*task)();
                    }
                    catch (std::runtime_error& e) {
                        LOG_ERROR("[THREAD POOL]: Task threw an exception" << e.what());
                    }
                    if (synchronize)
                    {
                        // Only decrement sync_tasks if it was a synchronized task
                        m_sync_tasks.fetch_sub(1, std::memory_order_release);
                        m_sync_condition.notify_all();
                    }
                    m_active_tasks.fetch_sub(1, std::memory_order_release);
                    });
                m_active_tasks.fetch_add(1, std::memory_order_acquire);
                if (synchronize)
                {
                    // Only increment sync_tasks if it's a synchronized task
                    m_sync_tasks.fetch_add(1, std::memory_order_acquire);
                }
            }

            m_condition.notify_one();
            return res;
        }

        void SyncThisThread(bool register_for_sync)
        {
            std::lock_guard<std::mutex> lock(m_sync_mutex);
            auto it = std::find_if(m_threads_control.begin(), m_threads_control.end(),
                [](const std::shared_ptr<ThreadControlBlock>& tcb) {
                    return tcb->thread_id == std::this_thread::get_id();
                });

            if (it != m_threads_control.end())
            {
                (*it)->is_registered_for_sync.store(register_for_sync, std::memory_order_release);
                (*it)->has_reached_sync_point.store(!register_for_sync, std::memory_order_release);
            }
            else
            {
                LOG_ERROR("[THREAD POOL]: Current thread is not part of the thread pool");
                assert(false);
            }
            m_sync_condition.notify_all();
        }

        bool SyncRegisteredTasks(std::chrono::milliseconds timeout = std::chrono::milliseconds(500))
        {
            std::unique_lock<std::mutex> lock(m_sync_mutex);
            auto start = std::chrono::steady_clock::now();
            bool synced = m_sync_condition.wait_for(lock, timeout, [this, &start] {
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

                // Only wait for sync_tasks to become 0, ignore non-synchronized tasks
                bool all_synced = (m_sync_tasks.load(std::memory_order_acquire) == 0);

                return all_synced;
                });

            if (synced) {
                for (auto& tcb : m_threads_control)
                {
                    tcb->is_registered_for_sync.store(false, std::memory_order_release);
                    tcb->has_reached_sync_point.store(false, std::memory_order_release);
                }
            }
            else {
                LOG_ERROR("[THREAD POOL]: Sync timed out after " << timeout.count() << "ms");
            }

            return synced;
        }

        void ExecuteAndWait(const std::vector<std::function<void()>>& tasks) {
            std::atomic<size_t> completed_tasks(0);
            size_t total_tasks = tasks.size();

            for (const auto& task : tasks) {
                Enqueue(TaskPriority::NORMAL, false, [&completed_tasks, task]() {
                    task();
                    completed_tasks.fetch_add(1, std::memory_order_release);
                    });
            }

            // Wait for all tasks to complete
            while (completed_tasks.load(std::memory_order_acquire) < total_tasks) {
                std::this_thread::yield();
            }
        }

        void WaitForAllTasks()
        {
            std::unique_lock<std::mutex> lock(m_queue_mutex);
            m_sync_condition.wait(lock, [this] {
                return m_active_tasks.load(std::memory_order_acquire) == 0 && AllQueuesEmpty();
                });
        }

    private:
        void Initialize(u32 num_threads)
        {
            m_tasks_queues.resize(num_threads);
            m_threads_control.resize(num_threads);

            for (u32 i = 0; i < num_threads; ++i)
            {
                m_threads_control[i] = std::make_shared<ThreadControlBlock>();
                m_workers.emplace_back(&ThreadPool::WorkerThread, this, m_threads_control[i], i);
            }
        }

        void Shutdown()
        {
            {
                std::unique_lock<std::mutex> lock(m_queue_mutex);
                m_stop = true;
            }
            m_condition.notify_all();
            for (std::thread& worker : m_workers)
            {
                worker.join();
            }
        }

        void WorkerThread(std::shared_ptr<ThreadControlBlock> tcb, u32 index)
        {
            tcb->thread_id = std::this_thread::get_id();

            while (true)
            {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(m_queue_mutex);
                    m_condition.wait(lock, [this, index] {
                        return m_stop || !m_tasks_queues[index].empty() || CanStealTask();
                        });

                    if (m_stop && AllQueuesEmpty())
                    {
                        return;
                    }

                    if (!m_tasks_queues[index].empty())
                    {
                        task = std::move(m_tasks_queues[index].front().second);
                        m_tasks_queues[index].pop_front();
                    }
                    else
                    {
                        task = StealTask();
                    }
                }

                if (task)
                {
                    task();
                }

                if (tcb->is_registered_for_sync.load(std::memory_order_acquire))
                {
                    tcb->has_reached_sync_point.store(true, std::memory_order_release);
                    m_sync_condition.notify_all();
                }
            }
        }

        bool CanStealTask() const
        {
            return std::any_of(m_tasks_queues.begin(), m_tasks_queues.end(),
                [](const auto& queue) { return !queue.empty(); });
        }

        std::function<void()> StealTask()
        {
            std::function<void()> task;
            for (auto& queue : m_tasks_queues)
            {
                if (!queue.empty())
                {
                    task = std::move(queue.front().second);
                    queue.pop_front();
                    break;
                }
            }
            return task;
        }

        bool AllQueuesEmpty() const
        {
            return std::all_of(m_tasks_queues.begin(), m_tasks_queues.end(),
                [](const auto& queue) { return queue.empty(); });
        }

        u32 SelectQueue()
        {
            static thread_local std::mt19937 gen(std::random_device{}());
            std::uniform_int_distribution<u32> dis(0, m_tasks_queues.size() - 1);
            return dis(gen);
        }

    private:
        std::vector<std::thread> m_workers;
        std::vector<std::shared_ptr<ThreadControlBlock>> m_threads_control;
        std::vector<std::deque<std::pair<TaskPriority, std::function<void()>>>> m_tasks_queues;
        std::mutex m_queue_mutex;
        std::condition_variable m_condition;
        std::mutex m_sync_mutex;
        std::condition_variable m_sync_condition;
        std::atomic<u64> m_active_tasks;
        std::atomic<u64> m_sync_tasks;
        std::atomic<bool> m_stop;
    };
} // namespace Spark

#endif // SPARK_THREAD_POOL_HPP