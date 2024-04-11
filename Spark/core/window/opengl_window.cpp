#include "opengl_window.hpp"

#include "../ecs/component/shader.hpp"
#include "window_manager.hpp"
#include "../events/sub.hpp"

namespace spark
{
	// ==============================================================================
	// EVENT CALLBACKS:

	void opengl_window::event_callback(std::shared_ptr <event> _event)
	{
		publish_to_topic(WINDOW_EVENT_TOPIC, _event);
	}

	void opengl_window::resized_event_callback(GLFWwindow* window, int32_t width, int32_t height)
	{
		opengl_window_data* _window_data = static_cast<opengl_window_data*>(glfwGetWindowUserPointer(window));
		_window_data->m_width = width;
		_window_data->m_height = height;

		glViewport(0, 0, width, height);

		auto event = std::make_shared<window_resized_event>(width, height);
		_window_data->m_event_callback(event);
	}

	void opengl_window::close_event_callback(GLFWwindow* window)
	{
		opengl_window_data* _window_data = static_cast<opengl_window_data*>(glfwGetWindowUserPointer(window));
		auto event = std::make_shared<window_closed_event>();

		_window_data->m_event_callback(event);
	}

	void opengl_window::key_event_callback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
	{
		opengl_window_data* _window_data = static_cast<opengl_window_data*>(glfwGetWindowUserPointer(window));

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

	void opengl_window::mouse_button_event_callback(GLFWwindow* window, int32_t button, int32_t action, int32_t mods)
	{
		opengl_window_data* _window_data = static_cast<opengl_window_data*>(glfwGetWindowUserPointer(window));

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

	void opengl_window::mouse_scroll_event_callback(GLFWwindow* window, float64_t xoffset, float64_t yoffset)
	{
		opengl_window_data* _window_data = static_cast<opengl_window_data*>(glfwGetWindowUserPointer(window));
		auto event = std::make_shared<mouse_scrolled_event>(xoffset, yoffset);

		_window_data->m_event_callback(event);
	}

	void opengl_window::mouse_move_event_callback(GLFWwindow* window, float64_t xpos, float64_t ypos)
	{
		opengl_window_data* _window_data = static_cast<opengl_window_data*>(glfwGetWindowUserPointer(window));
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

	void framebuffer_size_callback(GLFWwindow* window, int32_t width, int32_t height)
	{
		glViewport(0, 0, width, height);
	}

	// ==============================================================================
}