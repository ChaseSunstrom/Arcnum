#ifndef SPARK_DIRECTX_WINDOW_DATA_HPP
#define SPARK_DIRECTX_WINDOW_DATA_HPP

#include "../window_data.hpp"
#include "../../events/event.hpp"

namespace Spark
{

struct DirectXWindowData : public WindowData
{
    DirectXWindowData() = default;
    DirectXWindowData(std::string title, bool vsync, i32 height, i32 width,
                      std::function<void(std::shared_ptr<Event>)> event_callback)
        : WindowData(title, vsync, height, width, event_callback)
    {
    }

    HWND m_hwnd = nullptr;
    bool m_framebuffer_resized = false;
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_device_context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swap_chain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_render_target_view;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depth_stencil_view;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_input_layout;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertex_shader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixel_shader;
    D3D11_VIEWPORT m_viewport;
};
}

#endif