#ifndef SPARK_EVENT_HANDLER_HPP
#define SPARK_EVENT_HANDLER_HPP

#include <core/pch.hpp>
#include "event.hpp"

namespace Spark
{
	class EventHandler
	{
	public:
		EventHandler() = default;
		~EventHandler() = default;

		void SubscribeToEvent(i64 event_type, std::function<void(const std::shared_ptr<Event>)> handler) {
			m_event_handlers[event_type].push_back(handler);
		}

		void UnsubscribeFromEvent(i64 event_type) {
			m_event_handlers.erase(event_type);
		}

		void PublishEvent(i64 event_type, const std::shared_ptr<Event> event) {
			HandleEvent(event_type, event);
		}

		void ClearEventHandlers() {
			m_event_handlers.clear();
		}
	private:
		void HandleEvent(i64 event_type, const std::shared_ptr<Event> event) {
			auto event_fns = m_event_handlers[event_type];
			auto all_event_fns = m_event_handlers[EVENT_TYPE_ALL];

			for (const auto& fn : event_fns) {
				fn(event);
			}

			for (const auto& fn : all_event_fns) {
				fn(event);
			}
		}
	private:
		std::unordered_map<i64, std::vector<std::function<void(const std::shared_ptr<Event>)>>> m_event_handlers;
	};
}

#endif