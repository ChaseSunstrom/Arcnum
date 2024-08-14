#ifndef SPARK_EVENT_HPP
#define SPARK_EVENT_HPP

#include <core/pch.hpp>

#include "event_type.hpp"

namespace Spark
{
    struct IEvent
    {
        IEvent() = default;
        IEvent(EventType type) : m_type(type) {}
        virtual ~IEvent() = default;

        bool m_handled = false;
        EventType m_type = EventType::NONE;
    };
}

#endif