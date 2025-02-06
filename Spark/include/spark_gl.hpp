#ifndef SPARK_GL_HPP
#define SPARK_GL_HPP

#include "spark_pch.hpp"

#include "spark_event_queue.hpp"

namespace spark
{
	class WindowLayer;

	namespace opengl
	{
		struct SPARK_API GL
		{
			static void Initialize(WindowLayer& wl, EventQueue& eq, const std::string& title, const i32 win_width, const i32 win_height, bool win_vsync);
			static void Shutdown();
		};
	}
}


#endif