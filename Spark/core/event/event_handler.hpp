#ifndef SPARK_EVENT_HANDLER_HPP
#define SPARK_EVENT_HANDLER_HPP

#include <core/pch.hpp>
#include <core/util/thread_pool.hpp>
#include "event.hpp"

namespace Spark {
	struct FunctionSettings {
		bool threaded = false;
		bool wait     = false;
	};

	class EventHandler {
	  public:
		EventHandler(ThreadPool& thread_pool)
			: m_thread_pool(thread_pool) {}
		~EventHandler() = default;

		template<IsEvent _Ty> void SubscribeToEvent(Callable<void(const EventPtr<_Ty>&)> handler, const FunctionSettings settings = {}) {
			std::lock_guard<std::mutex> lock(m_mutex);
			
			m_single_event_handlers[typeid(_Ty)].PushBack({[handler](const EventPtr<IEvent>& base_event) {
								   if (auto derived_event = DynamicPointerCast<_Ty>(base_event)) {
									   handler(derived_event);
								   }
							   },
			                   settings});
		}

		template<IsEvent... EventTypes> void SubscribeToMultipleEvents(Callable<void(const MultiEventPtr<EventTypes...>&)> handler, const FunctionSettings settings = {}) {
			std::lock_guard<std::mutex> lock(m_mutex);
			auto                        wrapper = [handler](const EventPtr<IEvent>& base_event) {
                if ((DynamicPointerCast<EventTypes>(base_event) || ...)) {
                    auto multi_event = CreateMultiEvent<EventTypes...>(base_event);
                    if (multi_event) {
                        handler(multi_event);
                    }
                }
			};

			(RegisterMultiEventHandler<EventTypes>(wrapper, settings), ...);
		}

		void SubscribeToAllEvents(std::function<void(const EventPtr<IEvent>&)> handler, const FunctionSettings settings = {}) {
			std::lock_guard<std::mutex> lock(m_mutex);
			m_all_event_handlers.PushBack({handler, settings});
		}

		template<IsEvent _Ty> void PublishEvent(const EventPtr<_Ty>& event) {
			Vector<Pair<Callable<void(const EventPtr<IEvent>&)>, FunctionSettings>> handlers_to_call;

			{
				std::lock_guard<std::mutex> lock(m_mutex);

				auto single_it = m_single_event_handlers.Find(typeid(_Ty));
				if (single_it != m_single_event_handlers.End()) {
					handlers_to_call.Insert(handlers_to_call.End(), single_it->second.Begin(), single_it->second.End());
				}

				auto multi_it = m_multi_event_handlers.Find(typeid(_Ty));
				if (multi_it != m_multi_event_handlers.End()) {
					handlers_to_call.Insert(handlers_to_call.End(), multi_it->second.Begin(), multi_it->second.End());
				}

				handlers_to_call.Insert(handlers_to_call.End(), m_all_event_handlers.Begin(), m_all_event_handlers.End());
			}

			for (const auto& [handler, settings] : handlers_to_call) {
				if (settings.threaded)
					m_thread_pool.Enqueue(TaskPriority::VERY_HIGH, settings.wait, [handler, event]() { handler(event); });
				else {
					handler(event);
				}
			}
		}

	  private:
		template<IsEvent _Ty> void RegisterMultiEventHandler(const Callable<void(const EventPtr<IEvent>&)>& handler, const FunctionSettings& settings) {
			m_multi_event_handlers[typeid(_Ty)].PushBack({handler, settings});
		}

		template<IsEvent... EventTypes> static MultiEventPtr<EventTypes...> CreateMultiEvent(const EventPtr<IEvent>& base_event) { return MakeShared<MultiEvent<EventTypes...>>(base_event); }

		UnorderedMap<TypeIndex, Vector<Pair<Callable<void(const EventPtr<IEvent>&)>, FunctionSettings>>> m_single_event_handlers;
		UnorderedMap<TypeIndex, Vector<Pair<Callable<void(const EventPtr<IEvent>&)>, FunctionSettings>>> m_multi_event_handlers;
		Vector<Pair<Callable<void(const EventPtr<IEvent>&)>, FunctionSettings>>                          m_all_event_handlers;
		ThreadPool&                                                                                      m_thread_pool;
		std::mutex                                                                                       m_mutex;
	};
} // namespace Spark

#endif