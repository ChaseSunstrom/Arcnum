#ifndef SPARK_VULKAN_RENDERER_HPP
#define SPARK_VULKAN_RENDERER_HPP

#include "../renderer.hpp"
#include "../../window/vulkan/vulkan_window_data.hpp"

namespace Spark
{
	class VulkanRenderer : public Renderer
	{
      public:
		  VulkanRenderer() = default;

		  ~VulkanRenderer() = default;

		  void render(VulkanWindowData& window_data);
	};
}

#endif