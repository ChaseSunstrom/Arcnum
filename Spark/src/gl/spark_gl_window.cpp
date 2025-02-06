#include "spark_pch.hpp"
#include "opengl/spark_gl_window.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "spark_events.hpp"


namespace spark::opengl
{
	GLWindow::GLWindow(EventQueue& eq, const std::string& title, i32 width, i32 height, bool vsync)
		: Window(eq, title, width, height, vsync)
	{
		if (!glfwInit())
		{
			Logger::Logln(LogLevel::ERROR, "Failed to initialize GLFW");
			exit(EXIT_FAILURE);
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

		glfwSetWindowUserPointer(m_window, &m_data);

		if (!m_window)
		{
			Logger::Logln(LogLevel::ERROR, "Failed to create GLFW window");
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		glfwMakeContextCurrent(m_window);

		if (glewInit() != GLEW_OK)
		{
			Logger::Logln(LogLevel::ERROR, "Failed to initialize GLEW");
			exit(EXIT_FAILURE);
		}

		SetVSync(vsync);

		glfwSetWindowSizeCallback(m_window, 
			[](GLFWwindow* window, i32 width, i32 height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.width = width;
				data.height = height;
				data.eq.SubmitEvent<WindowResized>(width, height);
			});

	}
	
	GLWindow::~GLWindow()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void GLWindow::Update()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_window);
	}

	void GLWindow::Close()
	{
		glfwSetWindowShouldClose(m_window, GLFW_TRUE);
	}

	bool GLWindow::IsOpen() const
	{
		return !glfwWindowShouldClose(m_window);
	}

	void GLWindow::SetVSync(bool enabled)
	{
		glfwSwapInterval(enabled ? 1 : 0);
		m_data.vsync = enabled;
	}

	bool GLWindow::IsVSync() const
	{
		return m_data.vsync;
	}

	void GLWindow::SetTitle(const std::string& title)
	{
		m_data.title = title;
		glfwSetWindowTitle(m_window, title.c_str());
	}

	opaque GLWindow::GetNativeWindow() const
	{
		return m_window;
	}
}
