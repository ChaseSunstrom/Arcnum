#include "window.hpp"

#include "../events/sub.hpp"
#include "../events/event.hpp"
#include "../logging/log.hpp"

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

		vsync(m_window_data->m_vsync);

		//glfw callbacks
		glfwSetWindowSizeCallback(m_window, resized_event_callback);
		glfwSetWindowCloseCallback(m_window, close_event_callback);
		glfwSetKeyCallback(m_window, key_event_callback);
		glfwSetMouseButtonCallback(m_window, mouse_button_event_callback);
		glfwSetCursorPosCallback(m_window, mouse_move_event_callback);
		glfwSetScrollCallback(m_window, mouse_scroll_event_callback);
	}

	window::window()
	{
		m_window_data = std::make_unique<window_data>();
		init_gl();
	}

	window::window(std::string title, bool vsync, uint32_t height, uint32_t width)
	{
		m_window_data = std::make_unique<window_data>(title, vsync, height, width, window::event_callback);
		init_gl();
	}

	window_data::window_data()
	{
		m_event_callback = window::event_callback;
	}

	void window::on_update()
	{
		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}

	bool window::running()
	{
		return !glfwWindowShouldClose(m_window);
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
		auto event = std::make_shared<window_resized_event>(width, height);

		_window_data->m_width = width;
		_window_data->m_height = height;

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
