#ifndef SPARK_WINDOW_MANAGER_HPP
#define SPARK_WINDOW_MANAGER_HPP

#include "../spark.hpp"
#include "../app/api.hpp"
#include "directx/directx_window.hpp"
#include "metal/metal_window.hpp"
#include "vulkan/vulkan_window.hpp"
#include "window.hpp"

namespace Spark
{
template <typename W>
concept is_window_type = std::is_base_of_v<Window, W>;

class WindowManager
{
  public:
    static WindowManager &get()
    {
        static WindowManager instance;
        return instance;
    }

    template <is_window_type T> void add_window(T &window)
    {
        m_windows[typeid(T)] = &window;

        if (m_windows.size() == 1)
        {
            set_window<T>();
        }
    }

    template <is_window_type T> void set_window()
    {
        m_current_window = m_windows[typeid(T)];

        if (!m_current_window->initialized())
        {
            m_current_window->init();
        }
    }

    void set_window(API api)
    {
        switch (api)
        {
        case API::OPENGL:
            break;
        case API::VULKAN:
            set_window<VulkanWindow>();
            break;
        case API::DIRECTX:
            set_window<DirectXWindow>();
            break;
        case API::METAL:
            set_window<MetalWindow>();
            break;
        }
    }

    Window &get_current_window() const
    {
        return *m_current_window;
    }

  private:
    WindowManager()
    {
        auto &vk_window = Engine::get<VulkanWindow>();
        add_window(vk_window);
    }

    ~WindowManager() = default;

  private:
    Window *m_current_window;

    // Needs window pointers because you cant store references in an unordered_map
    // its a pointer to the static windows anyway, it should be fine
    std::unordered_map<std::type_index, Window *> m_windows;
};
} // namespace Spark

#endif