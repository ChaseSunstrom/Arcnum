#ifndef SPARK_GRAPHICS_API_HPP
#define SPARK_GRAPHICS_API_HPP

#include "spark_pch.hpp"
#include "spark_gl.hpp"
#include "spark_vk.hpp"
#include "spark_dx.hpp"

namespace spark
{
	template <typename ApiTy, typename CompTy>
	concept IsGraphicsApi = std::same_as<ApiTy, CompTy>;
}

#endif