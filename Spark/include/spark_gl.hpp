#ifndef SPARK_GL_HPP
#define SPARK_GL_HPP

#include "spark_pch.hpp"

namespace spark
{
	class WindowLayer;

	namespace opengl
	{
		struct SPARK_API GL
		{
			static void Initialize(WindowLayer& wl, const std::string& title, const i32 win_width, const i32 win_height, bool win_vsync);
			static void Shutdown();
		};
	}
}


#endif