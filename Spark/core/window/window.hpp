#ifndef SPARK_WINDOW_H
#define SPARK_WINDOW_H

#include "../events/event.hpp"
#include "../spark.hpp"
#include "../logging/log.hpp"


namespace spark
{
	struct window_data
	{
		window_data() = default;

		window_data(
			std::string title,
			bool vsync,
			i32 height,
			i32 width,
			std::function<void(std::shared_ptr<event>)> event_callback) :
			m_title(title), m_vsync(vsync), m_height(height), m_width(width), m_event_callback(event_callback) { }

		virtual ~window_data() = default;

		std::string m_title = "Title";

		bool m_vsync = false;

		i32 m_width = 1080;

		i32 m_height = 1080;

		std::function<void(std::shared_ptr<event>)> m_event_callback;
	};

	enum class window_type
	{
		UNKNOWN = 0,
		OPENGL,
		VULKAN,
		METAL,
		DIRECTX
	};

	class window
	{
	public:
		virtual void pre_draw() = 0;

		virtual void on_update() = 0;

		virtual void post_draw() = 0;

		virtual bool is_running() = 0;

		virtual void set_vsync(bool vsync) = 0;

		virtual void set_window_title(const std::string& title) = 0;

		virtual window_data& get_window_data() = 0;

		window_type get_window_type() { return m_type; }
	protected:
		window(window_type type) : m_type(type) {}

		virtual ~window() = default;

		bool m_running = true;

		window_type m_type = window_type::UNKNOWN;
	};

}

#endif // CORE_WINDOW_H