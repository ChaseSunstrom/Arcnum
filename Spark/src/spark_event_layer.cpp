#include "spark_pch.hpp"
#include "spark_event_layer.hpp"
#include "spark_application.hpp"

namespace spark
{
    void EventLayer::OnUpdate(DeltaTime<f64> dt)
    {
        // pass a reference to your “event systems” vector
        m_event_queue.DispatchAll(m_app, m_app.GetEventSystems());
    }
}