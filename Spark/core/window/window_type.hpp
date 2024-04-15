#ifndef SPARK_WINDOW_TYPE_HPP
#define SPARK_WINDOW_TYPE_HPP

#include "../spark.hpp"

namespace spark
{
	enum class window_type
	{
		UNKNOWN = 0, OPENGL, VULKAN, METAL, DIRECTX
	};

	window_type get_current_window_type();
}

#endif