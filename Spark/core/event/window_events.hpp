#ifndef SPARK_WINDOW_EVENTS_HPP
#define SPARK_WINDOW_EVENTS_HPP

#include "event.hpp"

namespace Spark
{
	struct WindowResizedEvent : public IEvent
	{
		WindowResizedEvent(i32 width, i32 height) : IEvent(EventType::WINDOW_RESIZE), width(width), height(height) {};
		i32 width;
		i32 height;
	};

	struct WindowClosedEvent : public IEvent
	{
		WindowClosedEvent() : IEvent(EventType::WINDOW_CLOSE) {};
	};
}

#endif