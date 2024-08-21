#ifndef SPARK_EVENT_HPP
#define SPARK_EVENT_HPP

#include <core/pch.hpp>

#include "event_type.hpp"

namespace Spark
{
    class Event
    {
    public:
        Event() = default;
        Event(i64 type) : m_type(type) {}
        virtual ~Event() = default;
    public:
        bool m_handled = false;
        i64 m_type = EVENT_TYPE_NONE;
    };
}

#endif