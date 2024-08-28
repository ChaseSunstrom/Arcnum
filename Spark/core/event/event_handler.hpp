#ifndef SPARK_EVENT_HANDLER_HPP
#define SPARK_EVENT_HANDLER_HPP

#include <core/pch.hpp>
#include "event.hpp"
#include <core/util/thread_pool.hpp>

namespace Spark
{
	struct FunctionSettings
	{
		bool threaded = false;
		bool wait = false;
	};

	class EventHandler
	{
	public:
		EventHandler(ThreadPool& thread_pool) : m_thread_pool(thread_pool) {}
		~EventHandler() = default;

		void SubscribeToEvent(i64 event_type, std::function<void(const std::shared_ptr<Event>)> handler, const FunctionSettings settings) {
			std::lock_guard<std::mutex> lock(m_mutex);
			m_event_handlers[event_type].push_back({ handler, settings });
		}

		void UnsubscribeFromEvent(i64 event_type) {
			std::lock_guard<std::mutex> lock(m_mutex);
			m_event_handlers.erase(event_type);
		}

		void PublishEvent(i64 event_type, const std::shared_ptr<Event> event) {
			std::vector<std::pair<std::function<void(const std::shared_ptr<Event>)>, FunctionSettings>> handlers_to_call;

			{
				std::lock_guard<std::mutex> lock(m_mutex);
				auto it = m_event_handlers.find(event_type);
				if (it != m_event_handlers.end()) {
					handlers_to_call = it->second;
				}
				auto all_it = m_event_handlers.find(EVENT_TYPE_ALL);
				if (all_it != m_event_handlers.end()) {
					handlers_to_call.insert(handlers_to_call.end(), all_it->second.begin(), all_it->second.end());
				}
			}

			for (const auto& [handler, settings] : handlers_to_call) {
				if (settings.threaded)
					m_thread_pool.Enqueue(TaskPriority::VERY_HIGH, settings.wait, [handler, event]() {
					handler(event);
						});
				else
					handler(event);
			}
		}

		void ClearEventHandlers() {
			std::lock_guard<std::mutex> lock(m_mutex);
			m_event_handlers.clear();
		}

	private:
		std::unordered_map<i64, std::vector<std::pair<std::function<void(const std::shared_ptr<Event>)>, FunctionSettings>>> m_event_handlers;
		ThreadPool& m_thread_pool;
		std::mutex m_mutex;
	};
}

#endif