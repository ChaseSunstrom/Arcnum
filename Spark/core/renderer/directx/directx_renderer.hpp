#ifndef SPARK_DIRECTX_RENDERER_HPP
#define SPARK_DIRECTX_RENDERER_HPP

#include "../../window/directx/directx_window_data.hpp"
#include "../renderer.hpp"

namespace Spark
{
class DirectXRenderer : public Renderer
{
  public:
    DirectXRenderer() = default;

    ~DirectXRenderer() = default;

    void render(DirectXWindowData &window_data);

    void toggle_wireframe_mode() override;

    void render_debugging_tools() override;
};
} // namespace Spark

#endif