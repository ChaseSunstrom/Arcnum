#ifndef SPARK_THREAD_POOL_HPP
#define SPARK_THREAD_POOL_HPP

#include "../spark.hpp"

namespace spark
{
    enum class task_priority
    {
        CRITICAL,
        VERY_HIGH,
        HIGH,
        NORMAL,
        LOW,
        VERY_LOW,
        BACKGROUND
    };

    // Comparator for the priority queue to sort based on task_priority
    struct task_comparator
    {
        bool operator()(const std::pair<task_priority, std::function<void()>>& lhs,
                        const std::pair<task_priority, std::function<void()>>& rhs) const
        {
            return lhs.first < rhs.first; // we want the queue to be a min-heap
        }
    };

    class thread_pool
    {
    public:

        thread_pool() = delete;
        ~thread_pool() = delete;

        static void initialize(uint32_t num_threads)
        {
            s_stop = false;
            s_active_tasks = 0;
            s_tasks_queues.resize(num_threads);

            for (uint32_t i = 0; i < num_threads; ++i)
            {
                s_workers.emplace_back(worker_thread, i);
            }
        }

        template <class F, class... Args>
        static auto enqueue(task_priority priority, F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type>
        {
            using return_type = typename std::invoke_result<F, Args...>::type;

            auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );

            std::future<return_type> res = task->get_future();
            {
                std::unique_lock<std::mutex> lock(s_queue_mutex);

                if (s_stop)
                {
                    throw std::runtime_error("Enqueue on stopped ThreadPool");
                }

                // Find the least loaded queue or with the highest priority task to insert this task
                auto min_it = std::min_element(s_tasks_queues.begin(), s_tasks_queues.end(),
                    [](const task_queue& a, const task_queue& b) {
                        return a.m_queue.size() < b.m_queue.size();
                    });

                min_it->m_queue.emplace(priority, [task]() { (*task)(); });
                s_active_tasks++;
            }

            s_condition.notify_one();
            return res;
        }

        static void synchronize()
        {
            std::unique_lock<std::mutex> lock(s_sync_mutex);
            s_sync_condition.wait(lock, [] 
                {
                    return s_active_tasks == 0;
                });
        }
    private:
            thread_pool(const thread_pool&) = delete;
            thread_pool& operator=(const thread_pool&) = delete;

            static void worker_thread(uint32_t thread_id)
            {
                while (true)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(s_queue_mutex);
                        s_condition.wait(lock, [thread_id] {
                                return s_stop || !s_tasks_queues[thread_id].m_queue.empty() || steal_task(thread_id);
                            });

                        if (s_stop && s_tasks_queues[thread_id].m_queue.empty() && !steal_task(thread_id))
                        {
                            return; // Exit point for the thread
                        }

                        task = s_tasks_queues[thread_id].m_queue.top().second;
                        s_tasks_queues[thread_id].m_queue.pop();
                    } // Release lock

                    task(); // Execute task

                    {
                        std::lock_guard<std::mutex> lock(s_sync_mutex);
                        s_active_tasks--;
                        if (s_active_tasks == 0)
                        {
                            // Notify synchronize() to proceed
                            s_sync_condition.notify_one();
                        }
                    }
                }
            }

            static bool steal_task(uint32_t thread_id)
            {
                // Iterate over other queues to steal a task
                for (uint32_t i = 0; i < s_tasks_queues.size(); ++i)
                {
                    if (i != thread_id) // Don't steal from itself
                    {
                        task_queue& queue = s_tasks_queues[i];
                        std::lock_guard<std::mutex> lock(queue.m_mutex);
                        if (!queue.m_queue.empty())
                        {
                            auto task = queue.m_queue.top();
                            queue.m_queue.pop();
                            s_tasks_queues[thread_id].m_queue.push(task);
                            return true;
                        }
                    }
                }
                return false;
            }
    private:
        struct task_queue
        {

            // Delete copy constructor and copy assignment operator
            task_queue(const task_queue&) = delete;
            task_queue& operator=(const task_queue&) = delete;

            // Implement the move constructor
            task_queue(task_queue&& other) noexcept
                : m_queue(std::move(other.m_queue))
            {
                // Note: mutex and other non-movable objects are not moved
            }

            // Implement the move assignment operator
            task_queue& operator=(task_queue&& other) noexcept
            {
                if (this != &other)
                {
                    m_queue = std::move(other.m_queue);
                    // Note: mutex and other non-movable objects are not moved
                }
                return *this;
            }

            // Default constructor
            task_queue() = default;

            std::priority_queue<
                std::pair<task_priority, std::function<void()>>,
                std::vector<std::pair<task_priority, std::function<void()>>>,
                task_comparator
            > m_queue;
            std::mutex m_mutex;
        };
        // Static members
        static inline std::vector<std::thread> s_workers;
        static inline std::vector<task_queue> s_tasks_queues;
        static inline std::mutex s_queue_mutex;
        static inline std::condition_variable s_condition;
        static inline std::mutex s_sync_mutex;
        static inline std::condition_variable s_sync_condition;
        static inline std::atomic<uint64_t> s_active_tasks;
        static inline std::atomic<bool> s_stop;
    };
}

#endif // SPARK_THREAD_POOL_HPP