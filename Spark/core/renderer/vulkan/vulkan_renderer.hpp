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

    void toggle_wireframe_mode() override;

    void render_debugging_tools() override;
};
} // namespace Spark

#endif