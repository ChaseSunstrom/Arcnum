#include "window.hpp"

#include "../events/sub.hpp"
#include "../events/event.hpp"
#include "../logging/log.hpp"
#include "../ui/ui.hpp"
#include "../util/wrap.hpp"
#include "../ecs/component/shader.hpp"

namespace spark
{

	// ==============================================================================
	// WINDOW FUNCTIONS:

	void window::init_gl()
	{
		glfwSetErrorCallback(glfw_error_callback);

		glfwInit();

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_window = glfwCreateWindow(
			m_window_data->m_width, m_window_data->m_height, m_window_data->m_title.c_str(), NULL, NULL);

		glfwMakeContextCurrent(m_window);
		glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
		glViewport(0, 0, m_window_data->m_width, m_window_data->m_height);
		glfwSetWindowUserPointer(m_window, m_window_data.get());
		glEnable(GL_DEPTH_TEST);

		glewInit();

		init_framebuffer();

		vsync(m_window_data->m_vsync);

		//glfw callbacks
		glfwSetWindowSizeCallback(m_window, resized_event_callback);
		glfwSetWindowCloseCallback(m_window, close_event_callback);
		glfwSetKeyCallback(m_window, key_event_callback);
		glfwSetMouseButtonCallback(m_window, mouse_button_event_callback);
		glfwSetCursorPosCallback(m_window, mouse_move_event_callback);
		glfwSetScrollCallback(m_window, mouse_scroll_event_callback);
	}

	void window::init_framebuffer()
	{
		if (m_window_data->m_screen_shader == 0)
		{
			auto& _shader_manager = engine::get<shader_manager>();
			auto [name, program] = _shader_manager.load_shader({ "Spark/shaders/quad.vert", "Spark/shaders/quad.frag" });
			m_window_data->m_screen_shader = program;
		}

		// Generate the framebuffer if not already created
		if (m_window_data->m_framebuffer == 0)
		{
			glGenFramebuffers(1, &m_window_data->m_framebuffer);
		}

		// Always bind the framebuffer to configure it
		glBindFramebuffer(GL_FRAMEBUFFER, m_window_data->m_framebuffer);

		// Create or resize the texture attached to the framebuffer
		if (m_window_data->m_texture_color_buffer == 0)
		{
			glGenTextures(1, &m_window_data->m_texture_color_buffer);
		}
		glBindTexture(GL_TEXTURE_2D, m_window_data->m_texture_color_buffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_window_data->m_width, m_window_data->m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_window_data->m_texture_color_buffer, 0);

		// Create or resize the render buffer attached to the framebuffer
		if (m_window_data->m_render_buffer == 0)
		{
			glGenRenderbuffers(1, &m_window_data->m_render_buffer);
		}
		glBindRenderbuffer(GL_RENDERBUFFER, m_window_data->m_render_buffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_window_data->m_width, m_window_data->m_height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_window_data->m_render_buffer);

		// Unbind the framebuffer after configuration
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		setup_fullscreen_quad();
	}

	void window::render_framebuffer_to_screen()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);

		glUseProgram(m_window_data->m_screen_shader);

		// Bind the VAO for the quad
		glBindVertexArray(m_window_data->m_quad_vao);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_window_data->m_texture_color_buffer);

		glUniform1i(glGetUniformLocation(m_window_data->m_screen_shader, "screenTexture"), 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
	}

	window& window::get()
	{
		static window instance;
		return instance;
	}

	window::window()
	{
		m_window_data = std::make_unique<window_data>();
		init_gl();
	}

	window_data::window_data()
	{
		m_event_callback = window::event_callback;
	}

	bool window::running()
	{
		return !glfwWindowShouldClose(m_window);
	}

	void window::pre_draw()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_window_data->m_framebuffer);
		clear_screen();
	}

	void window::post_draw()
	{
		glfwSwapBuffers(m_window); // Move buffer swap here.
		glfwPollEvents(); // Handle window events.
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void window::setup_fullscreen_quad()
	{
		static bool called = false;

		// So if the framebuffer gets recreated it does not do this again
		if (called) return;

		called = true;

		float32_t quad[] = {
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};

		glGenVertexArrays(1, &m_window_data->m_quad_vao);
		glGenBuffers(1, &m_window_data->m_quad_vbo);
		glBindVertexArray(m_window_data->m_quad_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_window_data->m_quad_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float32_t), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float32_t), (void*)(2 * sizeof(float32_t)));
	}

	void window::vsync(bool vsync)
	{
		if (vsync)
		{
			glfwSwapInterval(1);
		}

		else
		{
			glfwSwapInterval(0);
		}
	}

	void window::set_window_title(const std::string& title)
	{
		m_window_data->m_title = title;

		glfwSetWindowTitle(m_window, title.c_str());
	}

	window::~window()
	{
		m_running = false;
		glfwDestroyWindow(m_window);
		glDeleteFramebuffers(1, &m_window_data->m_framebuffer);
		glDeleteTextures(1, &m_window_data->m_texture_color_buffer);
		glDeleteRenderbuffers(1, &m_window_data->m_render_buffer);
		glfwDestroyWindow(m_window);
	}

	// ==============================================================================



	// ==============================================================================
	// EVENT CALLBACKS:

	void window::event_callback(std::shared_ptr <event> _event)
	{
		publish_to_topic(WINDOW_EVENT_TOPIC, _event);
	}

	void window::resized_event_callback(GLFWwindow* window, int32_t width, int32_t height)
	{
		window_data* _window_data = static_cast<window_data*>(glfwGetWindowUserPointer(window));
		_window_data->m_width = width;
		_window_data->m_height = height;

		get().init_framebuffer(); 

		glViewport(0, 0, width, height);

		auto event = std::make_shared<window_resized_event>(width, height);
		_window_data->m_event_callback(event);
	}

	void window::close_event_callback(GLFWwindow* window)
	{
		window_data* _window_data = static_cast<window_data*>(glfwGetWindowUserPointer(window));
		auto event = std::make_shared<window_closed_event>();

		_window_data->m_event_callback(event);
	}

	void window::key_event_callback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
	{
		window_data* _window_data = static_cast<window_data*>(glfwGetWindowUserPointer(window));

		switch (action)
		{
		case GLFW_PRESS:
		{
			auto event = std::make_shared<key_pressed_event>(key);
			_window_data->m_event_callback(event);
			break;
		}
		case GLFW_RELEASE:
		{
			auto event = std::make_shared<key_released_event>(key);
			_window_data->m_event_callback(event);
			break;
		}
		case GLFW_REPEAT:
		{
			auto event = std::make_shared<key_repeat_event>(key);
			_window_data->m_event_callback(event);
			break;
		}
		}
	}

	void window::mouse_button_event_callback(GLFWwindow* window, int32_t button, int32_t action, int32_t mods)
	{
		window_data* _window_data = static_cast<window_data*>(glfwGetWindowUserPointer(window));

		switch (action)
		{
		case GLFW_PRESS:
		{
			auto event = std::make_shared<mouse_pressed_event>(button);
			_window_data->m_event_callback(event);
			break;
		}
		case GLFW_RELEASE:
		{
			auto event = std::make_shared<mouse_released_event>(button);
			_window_data->m_event_callback(event);
			break;
		}
		}
	}

	void window::mouse_scroll_event_callback(GLFWwindow* window, float64_t xoffset, float64_t yoffset)
	{
		window_data* _window_data = static_cast<window_data*>(glfwGetWindowUserPointer(window));
		auto event = std::make_shared<mouse_scrolled_event>(xoffset, yoffset);

		_window_data->m_event_callback(event);
	}

	void window::mouse_move_event_callback(GLFWwindow* window, float64_t xpos, float64_t ypos)
	{
		window_data* _window_data = static_cast<window_data*>(glfwGetWindowUserPointer(window));
		auto event = std::make_shared<mouse_moved_event>(xpos, ypos);

		_window_data->m_event_callback(event);
	}

	// ==============================================================================



	// ==============================================================================
	// GLFW CALLBACKS:

	void glfw_error_callback(int32_t error, c_str description)
	{
		SPARK_ERROR("GLFW ERROR: " << description);
	}

	static void framebuffer_size_callback(GLFWwindow* window, int32_t width, int32_t height)
	{
		glViewport(0, 0, width, height);
	}

	// ==============================================================================
}
