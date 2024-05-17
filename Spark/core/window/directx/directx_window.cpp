#include "directx_window.hpp"
#include "../../events/sub.hpp"

namespace Spark
{
LRESULT CALLBACK window_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    auto window_data = reinterpret_cast<DirectXWindowData *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (message)
    {
    case WM_DESTROY: {
        auto event = std::make_shared<WindowClosedEvent>();
        window_data->m_event_callback(event);
        PostQuitMessage(0);
        return 0;
    }
    case WM_SIZE: {
        i32 width = LOWORD(lParam);
        i32 height = HIWORD(lParam);
        window_data->m_framebuffer_resized = true;
        break;
    }
    case WM_KEYDOWN: {
        i32 key = static_cast<i32>(wParam);
        auto event = std::make_shared<KeyPressedEvent>(key);
        window_data->m_event_callback(event);
        break;
    }
    case WM_KEYUP: {
        i32 key = static_cast<i32>(wParam);
        auto event = std::make_shared<KeyReleasedEvent>(key);
        window_data->m_event_callback(event);
        break;
    }
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN: {
        i32 button = 0;
        if (message == WM_LBUTTONDOWN)
            button = 0;
        else if (message == WM_RBUTTONDOWN)
            button = 1;
        else if (message == WM_MBUTTONDOWN)
            button = 2;
        auto event = std::make_shared<MousePressedEvent>(button);
        window_data->m_event_callback(event);
        break;
    }
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP: {
        i32 button = 0;
        if (message == WM_LBUTTONUP)
            button = 0;
        else if (message == WM_RBUTTONUP)
            button = 1;
        else if (message == WM_MBUTTONUP)
            button = 2;
        auto event = std::make_shared<MouseReleasedEvent>(button);
        window_data->m_event_callback(event);
        break;
    }
    case WM_MOUSEWHEEL: {
        i32 zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        f64 yoffset = static_cast<f64>(zDelta) / WHEEL_DELTA;
        auto event = std::make_shared<MouseScrolledEvent>(0.0, yoffset);
        window_data->m_event_callback(event);
        break;
    }
    case WM_MOUSEMOVE: {
        i32 xpos = GET_X_LPARAM(lParam);
        i32 ypos = GET_Y_LPARAM(lParam);
        auto event = std::make_shared<MouseMovedEvent>(xpos, ypos);
        window_data->m_event_callback(event);
        break;
    }
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}

DirectXWindow::DirectXWindow()
{
    m_window_data = std::make_unique<DirectXWindowData>("Title", false, 1080, 1080, event_callback);
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
    f32 clear_color[4] = {1.0f, 0.0f, 0.0f, 1.0f};
    m_window_data->m_device_context->ClearRenderTargetView(m_window_data->m_render_target_view.Get(), clear_color);


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

    const char *title_c_str = m_window_data->m_title.c_str();
    i32 len = MultiByteToWideChar(CP_UTF8, 0, title_c_str, -1, nullptr, 0);
    wchar_t *wide_title = new wchar_t[len];
    MultiByteToWideChar(CP_UTF8, 0, title_c_str, -1, wide_title, len);

    // Create the window
    m_window_data->m_hwnd =
        CreateWindowW(wc.lpszClassName, wide_title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                      m_window_data->m_width, m_window_data->m_height, nullptr, nullptr, wc.hInstance, nullptr);

    SetWindowLongPtr(m_window_data->m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(m_window_data.get()));

    delete[] wide_title;

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

void DirectXWindow::event_callback(std::shared_ptr<Event> event)
{
    publish_to_topic(WINDOW_EVENT_TOPIC, event);
}
} // namespace Spark