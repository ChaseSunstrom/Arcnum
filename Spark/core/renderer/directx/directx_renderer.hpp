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
};
} // namespace Spark

#endif