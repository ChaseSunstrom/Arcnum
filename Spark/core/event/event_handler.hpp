#ifndef SPARK_EVENT_HANDLER_HPP
#define SPARK_EVENT_HANDLER_HPP

#include "event.hpp"
#include <core/pch.hpp>
#include <core/util/thread_pool.hpp>

namespace Spark {
struct FunctionSettings {
	bool threaded = false;
	bool wait = false;
};

class EventHandler {
public:
	EventHandler(ThreadPool& thread_pool) : m_thread_pool(thread_pool) {}
	~EventHandler() = default;

	template<typename T>
	void SubscribeToEvent(std::function<void(const std::shared_ptr<T>&)> handler, const FunctionSettings settings = {}) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_event_handlers[typeid(T)].push_back({
			[handler](const std::shared_ptr<BaseEvent>& base_event) {
				handler(std::dynamic_pointer_cast<T>(base_event));
			},
			settings
		});
	}

	void SubscribeToAllEvents(std::function<void(const std::shared_ptr<BaseEvent>&)> handler, const FunctionSettings settings = {}) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_all_event_handlers.push_back({handler, settings});
	}

	template<typename... EventTypes>
	void SubscribeToMultipleEvents(std::function<void(const std::shared_ptr<BaseEvent>&)> handler, const FunctionSettings settings = {}) {
		(SubscribeToEvent<EventTypes>([handler](const std::shared_ptr<EventTypes>& event) {
			handler(std::static_pointer_cast<BaseEvent>(event));
		}, settings), ...);
	}

	void UnsubscribeFromEvent(const std::type_index& event_type) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_event_handlers.erase(event_type);
	}

	void UnsubscribeFromAllEvents() {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_all_event_handlers.clear();
	}

	template<typename T>
	void PublishEvent(const std::shared_ptr<T>& event) {
		std::vector<std::pair<std::function<void(const std::shared_ptr<BaseEvent>&)>, FunctionSettings>> handlers_to_call;

		{
			std::lock_guard<std::mutex> lock(m_mutex);
			auto it = m_event_handlers.find(typeid(T));
			if (it != m_event_handlers.end()) {
				handlers_to_call = it->second;
			}
			handlers_to_call.insert(handlers_to_call.end(), m_all_event_handlers.begin(), m_all_event_handlers.end());
		}

		for (const auto& [handler, settings] : handlers_to_call) {
			if (settings.threaded)
				m_thread_pool.Enqueue(TaskPriority::VERY_HIGH, settings.wait, [handler, event]() { handler(event); });
			else
				handler(event);
		}
	}

	void ClearEventHandlers() {
		std::lock_guard lock(m_mutex);
		m_event_handlers.clear();
		m_all_event_handlers.clear();
	}

private:
	std::unordered_map<std::type_index, std::vector<std::pair<std::function<void(const std::shared_ptr<BaseEvent>&)>, FunctionSettings>>> m_event_handlers;
	std::vector<std::pair<std::function<void(const std::shared_ptr<BaseEvent>&)>, FunctionSettings>> m_all_event_handlers;
	ThreadPool& m_thread_pool;
	std::mutex m_mutex;
};
}

#endif