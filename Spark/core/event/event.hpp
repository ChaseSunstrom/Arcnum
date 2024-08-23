#ifndef SPARK_EVENT_HPP
#define SPARK_EVENT_HPP

#include <core/pch.hpp>

#include "event_type.hpp"

namespace Spark
{
    struct Event
    {
    public:
        Event(i64 type = EVENT_TYPE_NONE) : type(type) {}
        virtual ~Event() = default;
    public:
        i64 type;
    };
}

#endif