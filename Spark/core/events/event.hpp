#ifndef SPARK_EVENT_HPP
#define SPARK_EVENT_HPP

#include "../spark.hpp"

namespace spark
{

	struct event
	{
		event() = default;

		event(i32 type) :
			m_type(type) { }

		virtual ~event() = default;

		bool m_handled = false;

		i32 m_type = NO_EVENT;
	};

	using app_update_event = event;
	using app_tick_event = event;
	using app_render_event = event;
	using window_closed_event = event;

	struct window_resized_event :
		event
	{
		window_resized_event() = default;

		window_resized_event(i32 width, i32 height) :
			event(WINDOW_RESIZED_EVENT), m_width(width), m_height(height) { }

		i32 m_width;

		i32 m_height;
	};

	struct window_moved_event :
		event
	{
		window_moved_event() = default;

		window_moved_event(i32 x_pos, i32 y_pos) :
			event(WINDOW_MOVED_EVENT), m_x_pos(x_pos), m_y_pos(y_pos) { }

		i32 m_x_pos;

		i32 m_y_pos;
	};

	struct key_pressed_event :
		event
	{
		key_pressed_event() = default;

		key_pressed_event(i32 key_code) :
			event(KEY_PRESSED_EVENT), m_key_code(key_code) { }

		i32 m_key_code;
	};

	struct key_released_event :
		event
	{
		key_released_event(i32 key_code) :
			event(KEY_RELEASED_EVENT), m_key_code(key_code) { }

		i32 m_key_code;
	};

	struct key_repeat_event :
		event
	{
		key_repeat_event() = default;

		key_repeat_event(i32 key_code) :
			event(KEY_REPEAT_EVENT), m_key_code(key_code) { }

		i32 m_key_code;
	};

	struct mouse_pressed_event :
		event
	{
		mouse_pressed_event() = default;

		mouse_pressed_event(i32 button) :
			event(MOUSE_PRESSED_EVENT), m_button(button) { }

		i32 m_button;
	};

	struct mouse_released_event :
		event
	{
		mouse_released_event() = default;

		mouse_released_event(i32 button) :
			event(MOUSE_RELEASED_EVENT), m_button(button) { }

		i32 m_button;
	};

	struct mouse_moved_event :
		event
	{
		mouse_moved_event() = default;

		mouse_moved_event(f64 x_pos, f64 y_pos) :
			event(MOUSE_MOVE_EVENT), m_x_pos(x_pos), m_y_pos(y_pos) { }

		f64 m_x_pos;

		f64 m_y_pos;
	};

	struct mouse_scrolled_event :
		event
	{
		mouse_scrolled_event() = default;

		mouse_scrolled_event(f64 x_offset, f64 y_offset) :
			event(MOUSE_SCROLLED_EVENT), m_x_offset(x_offset), m_y_offset(y_offset) { }

		f64 m_x_offset;

		f64 m_y_offset;
	};

	// ===============================================================

	class event_dispatcher
	{
	public:
		event_dispatcher() = default;

		event_dispatcher(std::shared_ptr <event> event) :
			m_event(event) { }

		inline bool dispatch(std::function<bool(std::shared_ptr<event>)> fn)
		{
			return fn(m_event);
		}

	private:
		std::shared_ptr <event> m_event;
	};
}

#endif // CORE_EVENT_H