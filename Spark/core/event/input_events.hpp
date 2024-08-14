#ifndef SPARK_INPUT_EVENTS_HPP
#define SPARK_INPUT_EVENTS_HPP

#include "event.hpp"

namespace Spark
{
	struct KeyPressedEvent : public IEvent
	{
		KeyPressedEvent(i32 key, i32 repeat) : IEvent(EventType::KEY_PRESSED), key(key) {};
		i32 key;
	};

	struct KeyReleasedEvent : public IEvent
	{
		KeyReleasedEvent(i32 key) : IEvent(EventType::KEY_RELEASED), key(key) {};
		i32 key;
	};

	struct KeyHeldEvent : public IEvent
	{
		KeyHeldEvent(i32 key) : IEvent(EventType::KEY_HELD), key(key) {};
		i32 key;
	};

	struct MouseMovedEvent : public IEvent
	{
		MouseMovedEvent(f64 xpos, f64 ypos) : IEvent(EventType::MOUSE_MOVED), xpos(xpos), ypos(ypos) {};
		f64 xpos;
		f64 ypos;
	};

	struct MouseButtonPressedEvent : public IEvent
	{
		MouseButtonPressedEvent(i32 button) : IEvent(EventType::MOUSE_BUTTON_PRESSED), button(button) {};
		i32 button;
	};

	struct MouseButtonReleasedEvent : public IEvent
	{
		MouseButtonReleasedEvent(i32 button) : IEvent(EventType::MOUSE_BUTTON_RELEASED), button(button) {};
		i32 button;
	};

	struct MouseScrolledEvent : public IEvent
	{
		MouseScrolledEvent(f64 x, f64 y) : IEvent(EventType::MOUSE_SCROLLED), x(x), y(y) {};
		f64 x;
		f64 y;
	};
}

#endif