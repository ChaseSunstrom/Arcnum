#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include "spark_pch.hpp"

// Namespace for the ThreadPool
namespace spark::threading {

    // Enum for Task Priority
    enum class TaskPriority { CRITICAL = 0, VERY_HIGH, HIGH, NORMAL, LOW, VERY_LOW, BACKGROUND };

    // Struct to hold the result of a task, including the executing thread's ID
    template <typename ReturnType>
    struct TaskResult {
        std::future<ReturnType> result;         // Future for the task's return value
        std::future<std::thread::id> thread_id; // Future for the executing thread's ID
    };

    // Struct to control individual worker threads
    struct ThreadControlBlock {
        std::thread::id thread_id;                  // ID of the worker thread
        std::atomic<bool> is_registered_for_sync{ false };   // Indicates if the thread is registered for synchronization
        std::atomic<bool> has_reached_sync_point{ false };   // Indicates if the thread has reached a synchronization point
    };

    // ThreadPool Class
    class ThreadPool {
    public:
        // Constructor: Initializes the pool with the specified number of threads
        ThreadPool(usize num_threads = std::thread::hardware_concurrency())
            : m_stop(false), m_active_tasks(0), m_rng(std::random_device{}()) {
            Initialize(num_threads);
        }

        ~ThreadPool() {
            Shutdown();
        }

        // Prevent copy and assignment
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;

        // Enqueue a task with a specified priority
        // Returns a TaskResult containing futures for the result and the executing thread's ID
        template<class F, class... Args>
        TaskResult<typename std::invoke_result<F, Args...>::type> Enqueue(TaskPriority priority, F&& f, Args&&... args) {
            using return_type = typename std::invoke_result<F, Args...>::type;

            auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );

            TaskResult<return_type> task_result;
            task_result.result = task->get_future();

            // Promise to set the thread ID after task execution
            auto thread_id_promise = std::make_shared<std::promise<std::thread::id>>();
            task_result.thread_id = thread_id_promise->get_future();

            {
                std::lock_guard<std::mutex> lock(m_queue_mutex);

                if (m_stop.load()) {
                    throw std::runtime_error("Enqueue on stopped ThreadPool");
                }

                usize queue_index = SelectQueue();

                m_task_queues[queue_index].emplace_back(priority, [task, thread_id_promise]() {
                    try {
                        (*task)();
                        thread_id_promise->set_value(std::this_thread::get_id());
                    }
                    catch (...) {
                        try {
                            thread_id_promise->set_value(std::this_thread::get_id());
                        }
                        catch (...) {
                        }
                    }
                    });

                m_active_tasks.fetch_add(1, std::memory_order_relaxed);
            }

            m_condition.notify_one();
            return task_result;
        }

        // Add more worker threads to the pool
        void AddThreads(usize count) {
            usize current_size = m_workers.size();
            m_task_queues.resize(current_size + count);
            m_thread_control.resize(current_size + count);

            for (usize i = current_size; i < current_size + count; ++i) {
                m_thread_control[i] = std::make_shared<ThreadControlBlock>();
                m_workers.emplace_back(&ThreadPool::WorkerThread, this, i);
            }
        }

        // Remove worker threads from the pool
        void RemoveThreads(usize count) {
            for (usize i = 0; i < count && !m_workers.empty(); ++i) {
                {
                    std::lock_guard<std::mutex> lock(m_queue_mutex);
                    m_stop.store(true);
                }
                m_condition.notify_all();

                if (m_workers.back().joinable()) {
                    m_workers.back().join();
                }
                m_workers.pop_back();
                m_task_queues.pop_back();
                m_thread_control.pop_back();

                m_stop.store(false);
            }
        }

        // Wait for all tasks to complete
        void WaitForAllTasks() {
            while (m_active_tasks.load(std::memory_order_relaxed) != 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }

        // DANGEROUS CALL
        void Shutdown() {
            m_stop.store(true);
            m_condition.notify_all();

            for (std::thread& worker : m_workers) {
                if (worker.joinable()) {
                    worker.join();
                }
            }
        }

    private:
        // Worker thread function
        void WorkerThread(usize index) {
            // Assign thread ID to control block
            m_thread_control[index]->thread_id = std::this_thread::get_id();

            while (true) {
                std::pair<TaskPriority, std::function<void()>> task;

                {
                    std::unique_lock<std::mutex> lock(m_queue_mutex);
                    m_condition.wait(lock, [this, index]() {
                        return m_stop.load() ||
                            !m_task_queues[index].empty() ||
                            std::any_of(m_task_queues.begin(), m_task_queues.end(), [](const auto& q) { return !q.empty(); });
                        });


                    if (m_stop.load() && m_active_tasks.load() == 0) {
                        return;
                    }

                    if (!m_task_queues[index].empty()) {
                        // Find the highest priority task
                        auto it = std::max_element(
                            m_task_queues[index].begin(),
                            m_task_queues[index].end(),
                            [](const std::pair<TaskPriority, std::function<void()>>& a,
                                const std::pair<TaskPriority, std::function<void()>>& b) -> bool {
                                    return a.first < b.first;
                            });

                        if (it != m_task_queues[index].end()) {
                            task = *it;
                            m_task_queues[index].erase(it);
                        }
                    }

                    // If no task found, attempt to steal
                    if (task.second == nullptr) {
                        if (StealTask(index, task)) {
                            // Successfully stole a task
                        }
                    }

                    if (task.second == nullptr) {
                        // No task found, continue waiting
                        continue;
                    }
                }

                // Execute the task outside the lock
                if (task.second) {
                    task.second();
                    m_active_tasks.fetch_sub(1, std::memory_order_relaxed);
                }
            }
        }

        // Steal a task from other queues
        bool StealTask(usize thief_index, std::pair<TaskPriority, std::function<void()>>& task) {
            for (usize i = 0; i < m_task_queues.size(); ++i) {
                if (i == thief_index) continue;

                if (!m_task_queues[i].empty()) {
                    // Steal the lowest priority task
                    auto it = std::min_element(
                        m_task_queues[i].begin(),
                        m_task_queues[i].end(),
                        [](const std::pair<TaskPriority, std::function<void()>>& a,
                            const std::pair<TaskPriority, std::function<void()>>& b) -> bool {
                                return a.first < b.first;
                        });

                    if (it != m_task_queues[i].end()) {
                        task = *it;
                        m_task_queues[i].erase(it);
                        return true;
                    }
                }
            }
            return false;
        }

        // Select a queue index for the current task
        usize SelectQueue() {
            std::lock_guard<std::mutex> lock(m_rng_mutex);
            std::uniform_int_distribution<usize> dist(0, m_task_queues.size() - 1);
            return dist(m_rng);
        }

        void Initialize(usize num_threads) {
            m_task_queues.resize(num_threads);
            m_thread_control.resize(num_threads);

            for (usize i = 0; i < num_threads; ++i) {
                m_thread_control[i] = std::make_shared<ThreadControlBlock>();
                m_workers.emplace_back(&ThreadPool::WorkerThread, this, i);
            }
        }

        // Data Members
        std::vector<std::thread> m_workers;  // Vector of worker threads
        std::vector<std::shared_ptr<ThreadControlBlock>> m_thread_control; // Control blocks for workers
        std::vector<std::deque<std::pair<TaskPriority, std::function<void()>>>> m_task_queues; // Per-thread task queues

        std::mutex m_queue_mutex;            // Mutex for task queues
        std::condition_variable m_condition; // Condition variable for task availability

        std::atomic<bool> m_stop;            // Flag to indicate pool shutdown
        std::atomic<unsigned long> m_active_tasks; // Number of active tasks

        // Random number generator for queue selection
        std::mt19937 m_rng;
        std::mutex m_rng_mutex;              // Mutex for RNG
    };


    // LockedRef provides exclusive (write) access.
    // Only one LockedRef may exist at a time.
    template <typename T>
    class LockedRef
    {
    public:
        // Immediately acquires an exclusive lock.
        LockedRef(T& obj, std::shared_mutex& mutex)
            : m_lock(mutex), m_obj(obj)
        {
        }

        // Implicit conversion to T&.
        operator T& ()
        {
            return m_obj;
        }

        // Arrow operator.
        T* operator->()
        {
            return &m_obj;
        }

		T& Get()
		{
			return m_obj;
		}

        LockedRef(LockedRef&& other) noexcept
            : m_lock(std::move(other.m_lock))
            , m_obj(other.m_obj)
        {
            // after move, other can't really hold the lock
        }

        LockedRef& operator=(LockedRef&& other) noexcept
        {
            m_lock = std::move(other.m_lock);
            m_obj = other.m_obj;
            return *this;
        }

    private:
        std::unique_lock<std::shared_mutex> m_lock;
        T& m_obj;
    };

    // LockedCRef provides shared (read) access.
    // Multiple LockedCRef objects may coexist.
    template <typename T>
    class LockedCRef
    {
    public:
        // Immediately acquires a shared lock.
        LockedCRef(const T& obj, std::shared_mutex& mutex)
            : m_lock(mutex), m_obj(obj)
        {
        }

        // Implicit conversion to const T&.
        operator const T& () const
        {
            return m_obj;
        }

        // Arrow operator.
        const T* operator->() const
        {
            return &m_obj;
        }

        LockedCRef(LockedCRef&& other) noexcept
            : m_lock(std::move(other.m_lock))
            , m_obj(other.m_obj)
        {
        }

        LockedCRef& operator=(LockedCRef&& other) noexcept
        {
            m_lock = std::move(other.m_lock);
            m_obj = other.m_obj;
            return *this;
        }


        T& Get()
        {
            return m_obj;
        }

    private:
        std::shared_lock<std::shared_mutex> m_lock;
        const T& m_obj;
    };

    // AutoLockable wraps any object with an internal shared mutex.
    template <typename T>
    class AutoLockable {
    public:
        template <typename ... Args>
        AutoLockable(Args && ... args)
            : m_obj(std::forward<Args>(args)...)
            , m_mutex(std::make_shared<std::shared_mutex>())
        {
        }

        LockedRef<T> Lock() {
            return LockedRef<T>(m_obj, *m_mutex);
        }

        LockedCRef<T> CLock() const {
            return LockedCRef<T>(m_obj, *m_mutex);
        }

        T& Get() {
            return m_obj;
        }

        const T& Get() const {
            return m_obj;
        }

    private:
        T m_obj;
        std::shared_ptr<std::shared_mutex> m_mutex;
    };

    template <typename T>
    class AutoLockable<T*> {
    public:
        explicit AutoLockable(T* ptr)
            : m_ptr(ptr)
            , m_mutex(std::make_shared<std::shared_mutex>())
        {
        }

        LockedRef<T> Lock() {
            return LockedRef<T>(*m_ptr, *m_mutex);
        }

        LockedCRef<T> CLock() const {
            return LockedCRef<T>(*m_ptr, *m_mutex);
        }

        T* Get() {
            return m_ptr;
        }

        const T* Get() const {
            return m_ptr;
        }

    private:
        T* m_ptr;
        std::shared_ptr<std::shared_mutex> m_mutex;
    };
}

#endif // THREAD_POOL_HPP
