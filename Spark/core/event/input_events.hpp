#ifndef SPARK_INPUT_EVENTS_HPP
#define SPARK_INPUT_EVENTS_HPP

#include "event.hpp"

namespace Spark
{
	struct KeyPressedEvent : public Event
	{
		KeyPressedEvent(i32 key, i32 repeat) : Event(EVENT_TYPE_KEY_PRESSED), key(key) {};
		i32 key;
	};

	struct KeyReleasedEvent : public Event
	{
		KeyReleasedEvent(i32 key) : Event(EVENT_TYPE_KEY_RELEASED), key(key) {};
		i32 key;
	};

	struct KeyHeldEvent : public Event
	{
		KeyHeldEvent(i32 key) : Event(EVENT_TYPE_KEY_HELD), key(key) {};
		i32 key;
	};

	struct MouseMovedEvent : public Event
	{
		MouseMovedEvent(f64 xpos, f64 ypos) : Event(EVENT_TYPE_MOUSE_MOVED), xpos(xpos), ypos(ypos) {};
		f64 xpos;
		f64 ypos;
	};

	struct MouseButtonPressedEvent : public Event
	{
		MouseButtonPressedEvent(i32 button) : Event(EVENT_TYPE_MOUSE_BUTTON_PRESSED), button(button) {};
		i32 button;
	};

	struct MouseButtonReleasedEvent : public Event
	{
		MouseButtonReleasedEvent(i32 button) : Event(EVENT_TYPE_MOUSE_BUTTON_RELEASED), button(button) {};
		i32 button;
	};

	struct MouseScrolledEvent : public Event
	{
		MouseScrolledEvent(f64 x, f64 y) : Event(EVENT_TYPE_MOUSE_SCROLLED), x(x), y(y) {};
		f64 x;
		f64 y;
	};
}

#endif