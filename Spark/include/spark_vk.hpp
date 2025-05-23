#ifndef SPARK_VK_HPP
#define SPARK_VK_HPP

#include "spark_pch.hpp"

namespace spark
{
	class WindowLayer;

	namespace vulkan
	{
		struct VK
		{
			static void Initialize(WindowLayer& app, const std::string& title, const i32 win_width, const i32 win_height, bool win_vsync);
			static void Shutdown();
		};
	}
}


#endif