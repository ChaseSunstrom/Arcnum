#ifndef SPARK_DIRECTX_WINDOW_HPP
#define SPARK_DIRECTX_WINDOW_HPP

#include "../../util/singleton.hpp"
#include "../window.hpp"

#include <d3d11.h>
#include <wrl/client.h>

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
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_device_context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swap_chain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_render_target_view;
};

class DirectXWindow : public Window
{
  public:
    static DirectXWindow &get()
    {
        static DirectXWindow instance;
        return instance;
    }
    void init() override;
    void pre_draw() override;
    void on_update() override;
    void post_draw() override;
    bool is_running() override;
    void set_vsync(bool vsync) override;
    void set_window_title(const std::string &title) override;
    DirectXWindowData &get_window_data() const override;

  private:
    DirectXWindow();
    virtual ~DirectXWindow();

    void init_window();
    void init_dx();
    void cleanup();

    std::unique_ptr<DirectXWindowData> m_window_data;
};

} // namespace Spark

#endif