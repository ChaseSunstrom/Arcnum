#ifndef SPARK_GRAPHICS_API_HPP
#define SPARK_GRAPHICS_API_HPP

#include "spark_pch.hpp"

namespace spark
{
	enum class GraphicsAPI
	{
		None = 0,
		OpenGL = 1,
		DirectX = 2,
		Vulkan = 3,
		Metal = 4
	};
}

#endif