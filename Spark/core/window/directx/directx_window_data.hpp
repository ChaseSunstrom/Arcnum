#ifndef SPARK_DIRECTX_WINDOW_DATA_HPP
#define SPARK_DIRECTX_WINDOW_DATA_HPP

#include "../window_data.hpp"
#include "../../events/event.hpp"

#include <d3d11.h>
#include <wrl/client.h>
#include <windowsx.h>

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
};

}

#endif