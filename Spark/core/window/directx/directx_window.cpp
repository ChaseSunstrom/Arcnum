#include "directx_window.hpp"

namespace Spark
{
LRESULT CALLBACK window_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    // Handle different window messages here
    // For example:
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    // Add more cases for other messages you want to handle
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
}

DirectXWindow::DirectXWindow()
{
    m_window_data = std::make_unique<DirectXWindowData>("Title", false, 1080, 1080, nullptr);
}

DirectXWindow::~DirectXWindow()
{
    cleanup();
}

void DirectXWindow::init()
{
    init_window();
    init_dx();
    m_initialized = true;
}

void DirectXWindow::pre_draw()
{
    // Perform any pre-draw tasks
}

void DirectXWindow::on_update()
{
    f32 clear_color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    m_window_data->m_device_context->ClearRenderTargetView(m_window_data->m_render_target_view.Get(), clear_color);

    // Render your scene here

    m_window_data->m_swap_chain->Present(m_window_data->m_vsync ? 1 : 0, 0);
}

void DirectXWindow::post_draw()
{
    // Perform any post-draw tasks
}

bool DirectXWindow::is_running()
{
    return m_running;
}

void DirectXWindow::set_vsync(bool vsync)
{
    m_window_data->m_vsync = vsync;
}

void DirectXWindow::set_window_title(const std::string &title)
{
    m_window_data->m_title = title;

    const char *title_c_str = title.c_str();
    int len = MultiByteToWideChar(CP_UTF8, 0, title_c_str, -1, nullptr, 0);
    wchar_t *wideTitle = new wchar_t[len];
    MultiByteToWideChar(CP_UTF8, 0, title_c_str, -1, wideTitle, len);

    SetWindowTextW(m_window_data->m_hwnd, wideTitle);

    delete[] wideTitle;
}

DirectXWindowData &DirectXWindow::get_window_data() const
{
    return *m_window_data;
}

void DirectXWindow::init_window()
{
    // Register the window class
    WNDCLASSW wc = {};
    wc.lpfnWndProc = window_proc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"DirectXWindowClass";
    RegisterClassW(&wc);

    const char *titleCStr = m_window_data->m_title.c_str();
    int len = MultiByteToWideChar(CP_UTF8, 0, titleCStr, -1, nullptr, 0);
    wchar_t *wideTitle = new wchar_t[len];
    MultiByteToWideChar(CP_UTF8, 0, titleCStr, -1, wideTitle, len);

    // Create the window
    m_window_data->m_hwnd =
        CreateWindowW(wc.lpszClassName, wideTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                      m_window_data->m_width, m_window_data->m_height, nullptr, nullptr, wc.hInstance, nullptr);

    delete[] wideTitle;

    ShowWindow(m_window_data->m_hwnd, SW_SHOW);
}

void DirectXWindow::init_dx()
{
    // Create the DirectX device and swap chain
    DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
    swap_chain_desc.BufferCount = 1;
    swap_chain_desc.BufferDesc.Width = m_window_data->m_width;
    swap_chain_desc.BufferDesc.Height = m_window_data->m_height;
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.OutputWindow = m_window_data->m_hwnd;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.Windowed = TRUE;

    D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
                                  &swap_chain_desc, &m_window_data->m_swap_chain, &m_window_data->m_device, nullptr,
                                  &m_window_data->m_device_context);

    // Create the render target view
    ID3D11Texture2D *back_buffer = nullptr;
    m_window_data->m_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&back_buffer));
    m_window_data->m_device->CreateRenderTargetView(back_buffer, nullptr, &m_window_data->m_render_target_view);
    back_buffer->Release();

    m_window_data->m_device_context->OMSetRenderTargets(1, m_window_data->m_render_target_view.GetAddressOf(), nullptr);

    // Set up the viewport
    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<f32>(m_window_data->m_width);
    viewport.Height = static_cast<f32>(m_window_data->m_height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_window_data->m_device_context->RSSetViewports(1, &viewport);
}

void DirectXWindow::cleanup()
{
    // Clean up DirectX resources
    m_window_data->m_render_target_view.Reset();
    m_window_data->m_swap_chain.Reset();
    m_window_data->m_device_context.Reset();
    m_window_data->m_device.Reset();

    // Destroy the window
    DestroyWindow(m_window_data->m_hwnd);
    m_window_data->m_hwnd = nullptr;
}
} // namespace Spark