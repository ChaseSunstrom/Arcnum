#include "gl_window.hpp"
#include <core/event/input_events.hpp>
#include <core/event/window_events.hpp>

namespace Spark
{
	GLWindow::GLWindow(i32 width, i32 height, const std::string& title, std::function<void(std::shared_ptr<IEvent>)> event_callback) : m_window_data(std::make_unique<WindowData>())
	{
		CreateWindow(width, height, title, event_callback);
		SetVSync(false);
	}

	GLWindow::~GLWindow()
	{
		DestroyWindow();
	}

	void GLWindow::CreateWindow(i32 width, i32 height, const std::string& title, std::function<void(std::shared_ptr<IEvent>)> event_callback)
	{
		m_window_data->width = width;
		m_window_data->height = height;
		m_window_data->title = title;
		m_window_data->event_callback = event_callback;

		glfwInit();

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_window = glfwCreateWindow(m_window_data->width, m_window_data->height, m_window_data->title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			// Failed to initialize GLAD
			return;
		}

		glfwSetWindowUserPointer(m_window, m_window_data.get());

		glfwSetWindowSizeCallback(m_window,
			[](GLFWwindow* window, i32 width, i32 height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.width = width;
				data.height = height;

				WindowResizedEvent event(width, height);
				data.event_callback(std::make_shared<WindowResizedEvent>(event));
			});

		glfwSetWindowCloseCallback(m_window,
			[](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				WindowClosedEvent event;
				data.event_callback(std::make_shared<WindowClosedEvent>(event));
			});

		glfwSetKeyCallback(m_window,
			[](GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.event_callback(std::make_shared<KeyPressedEvent>(event));
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.event_callback(std::make_shared<KeyReleasedEvent>(event));
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.event_callback(std::make_shared<KeyPressedEvent>(event));
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
					MouseButtonPressedEvent event(button);
					data.event_callback(std::make_shared<MouseButtonPressedEvent>(event));
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.event_callback(std::make_shared<MouseButtonReleasedEvent>(event));
					break;
				}
				}
			});

		glfwSetCursorPosCallback(m_window,
			[](GLFWwindow* window, f64 x, f64 y)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event(x, y);
				data.event_callback(std::make_shared<MouseMovedEvent>(event));
			});

		glfwSetScrollCallback(m_window,
			[](GLFWwindow* window, f64 x, f64 y)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event(x, y);
				data.event_callback(std::make_shared<MouseScrolledEvent>(event));
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