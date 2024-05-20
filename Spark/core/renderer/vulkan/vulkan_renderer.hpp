#ifndef SPARK_VULKAN_RENDERER_HPP
#define SPARK_VULKAN_RENDERER_HPP

#include "../../window/vulkan/vulkan_window_data.hpp"
#include "../renderer.hpp"

namespace Spark
{
class VulkanRenderer : public Renderer
{
  public:
    VulkanRenderer() = default;

    ~VulkanRenderer() = default;

    void render(VulkanWindowData &window_data);
};
} // namespace Spark

#endif