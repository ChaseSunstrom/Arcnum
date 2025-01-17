#ifndef SPARK_DX_HPP
#define SPARK_DX_HPP

#include "spark_pch.hpp"

namespace spark
{
	template <typename ApiTy>
	class WindowLayer;

	namespace directx
	{
		struct DX
		{
			static void Initialize(WindowLayer<DX>& app, const std::string& title, const i32 win_width, const i32 win_height, bool win_vsync);
			static void Shutdown();
		};
	}
}


#endif