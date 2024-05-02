#ifndef SPARK_THREAD_POOL_HPP
#define SPARK_THREAD_POOL_HPP

#include "../spark.hpp"
#include "../logging/log.hpp"

namespace spark
{
	enum class TaskPriority
	{
		CRITICAL, VERY_HIGH, HIGH, NORMAL, LOW, VERY_LOW, BACKGROUND
	};

	struct ThreadControlBlock
	{
		std::thread::id thread_id;
		std::atomic<bool> is_registered_for_sync { false };
		std::atomic<bool> has_reached_sync_point { false };
	};

	struct TaskComparator
	{
		bool operator()(
				const std::pair <TaskPriority, std::function<void()>>& lhs,
				const std::pair <TaskPriority, std::function<void()>>& rhs) const
		{
			return lhs.first < rhs.first; // Higher priority tasks first
		}
	};

	class ThreadPool
	{
	public:
		ThreadPool() = delete;

		~ThreadPool() = delete;

		static void initialize(u32 num_threads)
		{
			s_stop = false;
			s_active_tasks = 0;
			s_tasks_queues.resize(num_threads);
			s_threads_control.resize(num_threads);

			for (u32 i = 0; i < num_threads; ++i)
			{
				s_threads_control[i] = std::make_shared<ThreadControlBlock>();
				s_workers.emplace_back(worker_thread, s_threads_control[i], i);
			}
		}

		template <class F, class... Args>
		static auto enqueue(
				TaskPriority priority,
				bool synchronize,
				F&& f,
				Args&& ... args)->std::future<typename std::invoke_result<F, Args...>::type>
		{
			using return_type = typename std::invoke_result<F, Args...>::type;

			auto task = std::make_shared < std::packaged_task <
			            return_type() >> (std::bind(std::forward<F>(f), std::forward<Args>(args)...));

			std::future <return_type> res = task->get_future();
			{
				std::unique_lock <std::mutex> lock(s_queue_mutex);

				if (__SPARK_ASSERT__(s_stop))
				{
					SPARK_ERROR("[THREAD POOL]: enqueue() called after shutdown");
					return { };
				}

				// Find the least loaded queue or with the highest priority task to insert this task
				auto min_it = std::min_element(
						s_tasks_queues.begin(), s_tasks_queues.end(), [](const TaskQueue& a, const TaskQueue& b)
						{
							return a.m_queue.size() < b.m_queue.size();
						});

				min_it->m_queue.emplace(
						priority, [task, synchronize]()
						{
							(*task)();
							if (synchronize)
							{
								auto it = std::find_if(
										s_threads_control.begin(),
										s_threads_control.end(),
										[](const std::shared_ptr <ThreadControlBlock>& tcb)
										{
											return tcb->thread_id == std::this_thread::get_id();
										});
								if (it != s_threads_control.end())
								{
									(*it)->has_reached_sync_point = true;
								}
							}
						});
				s_active_tasks++;
			}

			s_condition.notify_one();
			return res;
		}

		static void sync_this_thread(bool register_for_sync)
		{
			auto it = std::find_if(
					s_threads_control.begin(),
					s_threads_control.end(),
					[](const std::shared_ptr <ThreadControlBlock>& tcb)
					{
						return tcb->thread_id == std::this_thread::get_id();
					});

			if (it != s_threads_control.end())
			{
				(*it)->is_registered_for_sync = register_for_sync;
			}
			else
			{
				// Thread is not part of the pool
				throw std::runtime_error("Current thread is not part of the thread pool");
			}
		}

		static void synchronize_registered_threads()
		{
			// Wait for all registered threads to reach their sync point
			std::unique_lock <std::mutex> lock(s_sync_mutex);
			s_sync_condition.wait(
					lock, []
					{
						return std::all_of(
								s_threads_control.begin(),
								s_threads_control.end(),
								[](const std::shared_ptr <ThreadControlBlock>& tcb)
								{
									return !tcb->is_registered_for_sync || tcb->has_reached_sync_point;
								});
					});

			// Reset sync points
			for (auto& tcb: s_threads_control)
			{
				tcb->has_reached_sync_point = false;
			}
		}

		static void shutdown()
		{
			{
				std::unique_lock <std::mutex> lock(s_queue_mutex);
				s_stop = true;
			}
			s_condition.notify_all(); // Wake up all threads to let them exit
		}

	private:
		static void worker_thread(std::shared_ptr <ThreadControlBlock> tcb, u32 index)
		{
			tcb->thread_id = std::this_thread::get_id();

			while (true)
			{
				std::function<void()> task;
				{
					std::unique_lock <std::mutex> lock(s_queue_mutex);
					s_condition.wait(
							lock, [index]
							{
								return s_stop || !s_tasks_queues[index].m_queue.empty();
							});

					if (s_stop && s_tasks_queues[index].m_queue.empty())
					{
						return; // Exit the thread if the pool is stopped and no tasks are left
					}

					std::lock_guard <std::mutex> queue_lock(s_tasks_queues[index].m_mutex);
					auto& queue = s_tasks_queues[index].m_queue;
					task = std::move(queue.top().second);
					queue.pop();
					s_active_tasks--;
				}

				if (task)
				{
					task(); // Execute the task
				}
			}
		}

	private:
		struct TaskQueue
		{
			TaskQueue(const TaskQueue&) = delete;

			TaskQueue& operator=(const TaskQueue&) = delete;

			TaskQueue(TaskQueue&& other)

			noexcept
					: m_queue(std::move(other.m_queue)) { }

			TaskQueue& operator=(TaskQueue&& other)

			noexcept
			{
				if (this != &other)
				{
					m_queue = std::move(other.m_queue);
				}
				return *this;
			}

			TaskQueue() = default;

			std::priority_queue <std::pair<TaskPriority, std::function < void()>>,
			std::vector <std::pair<TaskPriority, std::function < void()>>>,
			TaskComparator> m_queue;

			std::mutex m_mutex;
		};

		static inline std::vector <std::thread> s_workers;

		static inline std::vector <std::shared_ptr<ThreadControlBlock>> s_threads_control;

		static inline std::vector <TaskQueue> s_tasks_queues;

		static inline std::mutex s_queue_mutex;

		static inline std::condition_variable s_condition;

		static inline std::mutex s_sync_mutex;

		static inline std::condition_variable s_sync_condition;

		static inline std::atomic <u64> s_active_tasks { 0 };

		static inline std::atomic<bool> s_stop { false };
	};
}

#endif // SPARK_THREAD_POOL_HPP
