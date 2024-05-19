#include "directx_renderer.hpp"

namespace Spark
{
void DirectXRenderer::render(DirectXWindowData &window_data)
{

    f32 clear_color[4] = {1.0f, 0.0f, 0.0f, 1.0f};
    window_data.m_device_context->ClearRenderTargetView(window_data.m_render_target_view.Get(), clear_color);
}
} // namespace Spark