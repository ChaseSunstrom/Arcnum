#ifndef SPARK_WINDOW_H
#define SPARK_WINDOW_H

#include "../events/event.hpp"
#include "../spark.hpp"
#include "../logging/log.hpp"

namespace spark
{
	struct window_data
	{
		window_data();

		window_data(
				std::string title,
				bool vsync,
				int32_t height,
				int32_t width,
				std::function<void(std::shared_ptr<event>)> event_callback) :
				m_title(title), m_vsync(vsync), m_height(height), m_width(width), m_event_callback(event_callback) { }

		virtual ~window_data() = default;

		std::string m_title = "Title";

		bool m_vsync = false;

		int32_t m_width = 1080;

		int32_t m_height = 1080;

		std::function<void(std::shared_ptr<event>)> m_event_callback;
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
	protected:
		virtual ~window();

		bool m_running = true;
	};

}

#endif // CORE_WINDOW_H