#include "spark_pch.hpp"
#include "spark_window.hpp"

#include <GLFW/glfw3.h>

namespace spark
{
	Window::Window(EventQueue& eq, const std::string& title, i32 width, i32 height, bool vsync)
		: m_data(eq, title, width, height, vsync)
	{
	}

	EventQueue& Window::GetEventQueue() const
	{
		return m_data.eq;
	}


	i32 Window::GetWidth() const
	{
		return m_data.width;
	}

	i32 Window::GetHeight() const
	{
		return m_data.height;
	}

	const std::string& Window::GetTitle() const
	{
		return m_data.title;
	}
}