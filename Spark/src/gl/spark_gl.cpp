#include "spark_pch.hpp"
#include "spark_gl.hpp"
#include "opengl/spark_gl_window.hpp"

namespace spark::opengl
{
	void GL::Initialize(WindowLayer& wl, EventQueue& eq, const std::string& title, const i32 win_width, const i32 win_height, bool win_vsync)
	{
		wl.m_window = std::make_unique<GLWindow>(eq, title, win_width, win_height, win_vsync);
	}

	void GL::Shutdown()
	{
	}
}