#ifndef SPARK_WINDOW_EVENTS_HPP
#define SPARK_WINDOW_EVENTS_HPP

#include "event.hpp"

namespace Spark
{
struct WindowResizedEvent : public Event
{
	WindowResizedEvent(i32 width, i32 height) : Event(EVENT_TYPE_WINDOW_RESIZE), width(width), height(height) {};
	i32 width;
	i32 height;
};

struct WindowClosedEvent : public Event
{
	WindowClosedEvent() : Event(EVENT_TYPE_WINDOW_CLOSE) {};
};
} // namespace Spark

#endif