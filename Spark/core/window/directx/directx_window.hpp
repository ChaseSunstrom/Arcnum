#ifndef SPARK_DIRECTX_WINDOW_HPP
#define SPARK_DIRECTX_WINDOW_HPP

#include "../../util/singleton.hpp"
#include "../../renderer/directx/directx_renderer.hpp"
#include "../window.hpp"
#include "directx_window_data.hpp"

namespace Spark
{
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
    void draw() override;
    void post_draw() override;
    bool is_running() const override;
    void set_vsync(bool vsync) override;
    void set_window_title(const std::string &title) override;
    static void event_callback(std::shared_ptr<Event> event);
    DirectXWindowData &get_window_data() const override;
    DirectXRenderer &get_renderer() const;

  private:
    DirectXWindow();
    virtual ~DirectXWindow();

    void init_window();
    void init_dx();
    void cleanup();

    std::unique_ptr<DirectXWindowData> m_window_data;
    std::unique_ptr<DirectXRenderer> m_renderer = std::make_unique<DirectXRenderer>();
};

} // namespace Spark

#endif