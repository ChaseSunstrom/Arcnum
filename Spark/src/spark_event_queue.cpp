#include "spark_pch.hpp"
#include "spark_event_queue.hpp"
#include "spark_application.hpp"

namespace spark
{
    void EventQueue::DispatchAll(Application& app, std::vector<std::unique_ptr<detail::ISystem>>& event_systems)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        while (!m_events.empty())
        {
            auto evt_ptr = m_events.front();
            m_events.pop();

            // Subscription callbacks:
            evt_ptr->VisitActive([this, base_evt = evt_ptr.get()](std::type_index tid, void* ptr)
                {
                    for (auto& sub : m_subscriptions)
                    {
                        sub.fn(base_evt, tid, ptr);
                    }
                });

            // Also aggregator-based event systems:
            for (auto& sys_ptr : event_systems)
            {
                sys_ptr->Execute(app, evt_ptr);
            }
        }
    }
}