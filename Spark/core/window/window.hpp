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
				uint32_t height,
				uint32_t width,
				std::function<void(std::shared_ptr<event>)> event_callback) :
				m_title(title), m_vsync(vsync), m_height(height), m_width(width), m_event_callback(event_callback) { }

		~window_data() = default;

		std::string m_title = "Title";

		bool m_vsync = false;

		uint32_t m_width = 1080;

		uint32_t m_height = 1080;

		std::function<void(std::shared_ptr<event > )> m_event_callback;
	};

	class window
	{
	public:
		window();

		window(std::string title, bool vsync, uint32_t height, uint32_t width);

		~window();

		void on_update();

		void vsync(bool vsync);

		void init_gl();

		bool running();

		void set_window_title(const std::string& title);

		// ==============================================================================
		// EVENT CALLBACKS:

		static void event_callback(std::shared_ptr<event> event);

		static void resized_event_callback(GLFWwindow* window, int32_t width, int32_t height);

		static void close_event_callback(GLFWwindow* window);

		static void key_event_callback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods);

		static void mouse_button_event_callback(GLFWwindow* window, int32_t button, int32_t action, int32_t mods);

		static void mouse_scroll_event_callback(GLFWwindow* window, float64_t xoffset, float64_t yoffset);

		static void mouse_move_event_callback(GLFWwindow* window, float64_t x, float64_t y);

		// ==============================================================================

	private:

		GLFWwindow* m_window;

		std::unique_ptr <window_data> m_window_data = std::make_unique<window_data>();

		bool m_running = true;
	};

	// ==============================================================================
	// GLFW CALLBACKS:

	void glfw_error_callback(int32_t error, c_str description);

	void framebuffer_size_callback(GLFWwindow* window, int32_t width, int32_t height);

	// ==============================================================================
}

#endif // CORE_WINDOW_H