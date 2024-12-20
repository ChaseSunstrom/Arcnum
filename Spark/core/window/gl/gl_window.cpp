#include "gl_window.hpp"
#include <core/event/event.hpp>
#include <core/util/log.hpp>

namespace Spark {
	GLWindow::GLWindow(const String& title, i32 width, i32 height, EventHandler& event_handler, bool vsync)
		: Window(title, width, height, event_handler, vsync) {
		CreateWindow(width, height, title);
		SetVSync(vsync);

		m_framebuffer = MakeUnique<GLFramebuffer>(width, height);

		// Subscribes to make sure window gets resized properly
		event_handler.SubscribeToEvent<WindowResizedEvent>([this](const EventPtr<WindowResizedEvent>& event) { SetSize(event->width, event->height); });
	}

	GLWindow::~GLWindow() { DestroyWindow(); }

	void GLWindow::CreateWindow(i32 width, i32 height, const String& title) {
		m_window_data->width  = width;
		m_window_data->height = height;
		m_window_data->title  = title;

		glfwInit();

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_window = glfwCreateWindow(m_window_data->width, m_window_data->height, m_window_data->title.CStr(), nullptr, nullptr);
		glfwMakeContextCurrent(m_window);

		if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
			// Failed to initialize GLAD
			LOG_FATAL("Failed to initialize glad!");
		}

		glfwSetWindowUserPointer(m_window, m_window_data.Get());

		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, i32 width, i32 height) {
			WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);
			data.width       = width;
			data.height      = height;
			data.event_handler.PublishEvent<WindowResizedEvent>(MakeEvent<WindowResizedEvent>(width, height));
		});

		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
			WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

			data.event_handler.PublishEvent<WindowClosedEvent>(MakeEvent<WindowClosedEvent>());
		});

		glfwSetKeyCallback(m_window, [](GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
			WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

			switch (action) {
				case GLFW_PRESS: {
					data.event_handler.PublishEvent<KeyPressedEvent>(MakeEvent<KeyPressedEvent>(key, 0));
					break;
				}
				case GLFW_RELEASE: {
					data.event_handler.PublishEvent<KeyReleasedEvent>(MakeEvent<KeyReleasedEvent>(key));
					break;
				}
				case GLFW_REPEAT: {
					data.event_handler.PublishEvent<KeyHeldEvent>(MakeEvent<KeyHeldEvent>(key));
					break;
				}
			}
		});

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, i32 button, i32 action, i32 mods) {
			WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

			switch (action) {
				case GLFW_PRESS: {
					data.event_handler.PublishEvent<MouseButtonPressedEvent>(MakeEvent<MouseButtonPressedEvent>(button));
					break;
				}
				case GLFW_RELEASE: {
					data.event_handler.PublishEvent<MouseButtonReleasedEvent>(MakeEvent<MouseButtonReleasedEvent>(button));
					break;
				}
			}
		});

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, f64 x, f64 y) {
			WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

			data.event_handler.PublishEvent<MouseMovedEvent>(MakeEvent<MouseMovedEvent>(x, y));
		});

		glfwSetScrollCallback(m_window, [](GLFWwindow* window, f64 x, f64 y) {
			WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

			data.event_handler.PublishEvent<MouseScrolledEvent>(MakeEvent<MouseScrolledEvent>(x, y));
		});
	}

	void GLWindow::DestroyWindow() {
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void GLWindow::Update() {
		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}

	bool GLWindow::Running() { return !glfwWindowShouldClose(m_window); }

	void GLWindow::SetTitle(const String& title) {
		m_window_data->title = title;
		glfwSetWindowTitle(m_window, m_window_data->title.CStr());
	}

	void GLWindow::SetSize(i32 width, i32 height) {
		m_window_data->width  = width;
		m_window_data->height = height;
		glfwSetWindowSize(m_window, m_window_data->width, m_window_data->height);
		m_framebuffer->Resize(width, height);
	}

	void GLWindow::SetVSync(bool enabled) {
		m_window_data->vsync = enabled;
		glfwSwapInterval(m_window_data->vsync);
	}

	bool GLWindow::IsVSync() const { return m_window_data->vsync; }

	WindowData& GLWindow::GetWindowData() const { return *m_window_data; }

	GLFramebuffer& GLWindow::GetFrameBuffer() const { return *m_framebuffer; }

	GLFWwindow* GLWindow::GetNativeWindow() const { return m_window; }
} // namespace Spark