#ifndef SPARK_EVENTS_HPP
#define SPARK_EVENTS_HPP

#include "spark_pch.hpp"

namespace spark
{
	struct KeyPressed
	{
		KeyPressed(i32 key, i32 repeat)
			: key(key), repeat(repeat) {}

		i32 key;
		i32 repeat;
	};

	struct KeyReleased
	{
		KeyReleased(i32 key) : key(key) {}

		i32 key;
	};

	struct KeyHeld
	{
		KeyHeld(i32 key) : key(key) {}

		i32 key;
	};

	struct MouseMoved
	{
		MouseMoved(f64 x, f64 y) : x(x), y(y) {}

		f64 x;
		f64 y;
	};

	struct MouseButtonPressed
	{
		MouseButtonPressed(i32 button) : button(button) {}

		i32 button;
	};

	struct MouseButtonReleased
	{
		MouseButtonReleased(i32 button) : button(button) {}

		i32 button;
	};

	struct MouseScrolled
	{
		MouseScrolled(f64 x, f64 y) : x(x), y(y) {}

		f64 x;
		f64 y;
	};

	struct WindowResized
	{
		WindowResized(i32 width, i32 height) : width(width), height(height) {}

		i32 width;
		i32 height;
	};

	struct WindowClosed {};
}


#endif