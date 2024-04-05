#ifndef SPARK_OPENGL_WINDOW_HPP
#define SPARK_OPENGL_WINDOW_HPP

#include "window.hpp"

namespace spark
{
	struct opengl_window_data : window_data
	{
		opengl_window_data() = default;

		opengl_window_data(std::string title,
			bool vsync,
			int32_t height,
			int32_t width,
			std::function<void(std::shared_ptr<event>)> event_callback) :
			window_data(title, vsync, height, width, event_callback) {}
		
		GLuint m_framebuffer = 0;

		GLuint m_texture_color_buffer = 0;

		GLuint m_render_buffer = 0;

		GLuint m_quad_vao = 0;

		GLuint m_quad_vbo = 0;

		GLuint m_screen_shader = 0;

	};

	class opengl_window : public window
	{
	public:
		static opengl_window& get();

		void pre_draw() override;

		void on_update()  override;

		void post_draw()  override;
		
		bool is_running() override;
		
		void set_vsync(bool vsync) override;
		
		void set_window_title(const std::string& title) override;

		void render_framebuffer_to_screen();

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
		opengl_window();

		void setup_fullscreen_quad();

		void init_gl();

		void init_framebuffer();

		~opengl_window();
	private:
		GLFWwindow* m_window;

		std::unique_ptr<opengl_window_data> m_window_data = std::make_unique<opengl_window_data>();
	};


	// ==============================================================================
	// GLFW CALLBACKS:

	void glfw_error_callback(int32_t error, c_str description);

	void framebuffer_size_callback(GLFWwindow* window, int32_t width, int32_t height);

	// ==============================================================================
}

#endif