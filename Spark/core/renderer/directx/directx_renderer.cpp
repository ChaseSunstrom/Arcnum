#include "directx_renderer.hpp"
#include "../../ecs/ecs.hpp"
#include "../../window/directx/directx_mesh.hpp"

namespace Spark
{
void DirectXRenderer::render(DirectXWindowData &window_data)
{
    const float clear_color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    window_data.m_device_context->ClearRenderTargetView(window_data.m_render_target_view.Get(), clear_color);

    // Clear the depth buffer
    window_data.m_device_context->ClearDepthStencilView(window_data.m_depth_stencil_view.Get(), D3D11_CLEAR_DEPTH, 1.0f,
                                                        0);

    // Set the render target
    window_data.m_device_context->OMSetRenderTargets(1, window_data.m_render_target_view.GetAddressOf(),
                                                     window_data.m_depth_stencil_view.Get());

    // Set the viewport
    window_data.m_device_context->RSSetViewports(1, &window_data.m_viewport);

    // Iterate through all renderables
    for (auto &[mesh_name, material_map] : m_renderables)
    {
        for (auto &[material_name, transforms_ptr] : material_map)
        {
            auto &mesh = Engine::get<MeshManager>().get_mesh(mesh_name);

            auto &directx_mesh = dynamic_cast<DirectXMesh &>(mesh);

            // Set vertex buffer
            UINT stride = sizeof(Vertex);
            UINT offset = 0;
            window_data.m_device_context->IASetVertexBuffers(0, 1, directx_mesh.m_vertex_buffer.GetAddressOf(), &stride,
                                                             &offset);

            // Set index buffer if it exists
            if (!directx_mesh.m_indices.empty())
            {
                window_data.m_device_context->IASetIndexBuffer(directx_mesh.m_index_buffer.Get(), DXGI_FORMAT_R32_UINT,
                                                               0);
            }

            // Set input layout
            window_data.m_device_context->IASetInputLayout(window_data.m_input_layout.Get());

            // Set primitive topology
            window_data.m_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            // Set shaders
            window_data.m_device_context->VSSetShader(window_data.m_vertex_shader.Get(), nullptr, 0);
            window_data.m_device_context->PSSetShader(window_data.m_pixel_shader.Get(), nullptr, 0);

            // Bind constant buffers and other resources
            directx_mesh.update_constant_buffers();
            window_data.m_device_context->VSSetConstantBuffers(0, 1,
                                                               directx_mesh.m_cb_list[0]->m_buffer.GetAddressOf());
            window_data.m_device_context->PSSetConstantBuffers(0, 1,
                                                               directx_mesh.m_cb_list[0]->m_buffer.GetAddressOf());

            // Draw call
            if (!directx_mesh.m_indices.empty())
            {
                window_data.m_device_context->DrawIndexed(static_cast<UINT>(directx_mesh.m_indices.size()), 0, 0);
            }
            else
            {
                window_data.m_device_context->Draw(static_cast<UINT>(directx_mesh.m_vertices.size()), 0);
            }
        }
    }
}

void DirectXRenderer::toggle_wireframe_mode()
{
}

void DirectXRenderer::render_debugging_tools()
{
}
} // namespace Spark