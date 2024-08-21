#include "gl_window.hpp"
#include <core/event/input_events.hpp>
#include <core/event/window_events.hpp>
#include <core/util/log.hpp>

namespace Spark
{
	GLWindow::GLWindow(const std::string& title, i32 width, i32 height, EventHandler& event_handler, bool vsync) : Window(title, width, height, event_handler, vsync)
	{
		CreateWindow(width, height, title);
		SetVSync(vsync);
	}

	GLWindow::~GLWindow()
	{
		DestroyWindow();
	}

	void GLWindow::CreateWindow(i32 width, i32 height, const std::string& title)
	{
		m_window_data->width = width;
		m_window_data->height = height;
		m_window_data->title = title;

		glfwInit();

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_window = glfwCreateWindow(m_window_data->width, m_window_data->height, m_window_data->title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			// Failed to initialize GLAD
			LOG_ERROR("Failed to initialize glad!");
			assert(false);
		}

		glfwSetWindowUserPointer(m_window, m_window_data.get());

		glfwSetWindowSizeCallback(m_window,
			[](GLFWwindow* window, i32 width, i32 height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.width = width;
				data.height = height;

				data.event_handler.PublishEvent(EVENT_TYPE_WINDOW_RESIZE, std::make_shared<WindowResizedEvent>(width, height));
			});

		glfwSetWindowCloseCallback(m_window,
			[](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				data.event_handler.PublishEvent(EVENT_TYPE_WINDOW_CLOSE, std::make_shared<WindowClosedEvent>());
			});

		glfwSetKeyCallback(m_window,
			[](GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					data.event_handler.PublishEvent(EVENT_TYPE_KEY_PRESSED, std::make_shared<KeyPressedEvent>(key, 0));
					break;
				}
				case GLFW_RELEASE:
				{
					data.event_handler.PublishEvent(EVENT_TYPE_KEY_RELEASED, std::make_shared<KeyReleasedEvent>(key));
					break;
				}
				case GLFW_REPEAT:
				{
					data.event_handler.PublishEvent(EVENT_TYPE_KEY_HELD, std::make_shared<KeyPressedEvent>(key, 1));
					break;
				}
				}
			});

		glfwSetMouseButtonCallback(m_window,
			[](GLFWwindow* window, i32 button, i32 action, i32 mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					data.event_handler.PublishEvent(EVENT_TYPE_MOUSE_BUTTON_PRESSED, std::make_shared<MouseButtonPressedEvent>(button));
					break;
				}
				case GLFW_RELEASE:
				{
					data.event_handler.PublishEvent(EVENT_TYPE_MOUSE_BUTTON_RELEASED,std::make_shared<MouseButtonReleasedEvent>(button));
					break;
				}
				}
			});

		glfwSetCursorPosCallback(m_window,
			[](GLFWwindow* window, f64 x, f64 y)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				data.event_handler.PublishEvent(EVENT_TYPE_MOUSE_MOVED, std::make_shared<MouseMovedEvent>(x, y));
			});

		glfwSetScrollCallback(m_window,
			[](GLFWwindow* window, f64 x, f64 y)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				data.event_handler.PublishEvent(EVENT_TYPE_MOUSE_SCROLLED, std::make_shared<MouseScrolledEvent>(x, y));
			});
	}

	void GLWindow::DestroyWindow()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void GLWindow::Update()
	{
		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}

	bool GLWindow::Running()
	{
		return !glfwWindowShouldClose(m_window);
	}

	void GLWindow::SetTitle(const std::string& title)
	{
		m_window_data->title = title;
		glfwSetWindowTitle(m_window, m_window_data->title.c_str());
	}

	void GLWindow::SetSize(i32 width, i32 height)
	{
		m_window_data->width = width;
		m_window_data->height = height;
		glfwSetWindowSize(m_window, m_window_data->width, m_window_data->height);
	}

	void GLWindow::SetVSync(bool enabled)
	{
		m_window_data->vsync = enabled;
		glfwSwapInterval(m_window_data->vsync);
	}

	bool GLWindow::IsVSync() const
	{
		return m_window_data->vsync;
	}

	WindowData&	GLWindow::GetWindowData() const
	{
		return *m_window_data;
	}

	GLFWwindow* GLWindow::GetNativeWindow() const
	{
		return m_window;
	}
}