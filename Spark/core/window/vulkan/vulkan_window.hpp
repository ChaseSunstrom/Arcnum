#ifndef SPARK_VULKAN_WINDOW_HPP
#define SPARK_VULKAN_WINDOW_HPP

#include "../../util/singleton.hpp"
#include "../window.hpp"
#include "../../renderer/vulkan/vulkan_renderer.hpp"
#include "vulkan_window_data.hpp"

namespace Spark
{
namespace Internal
{
struct VulkanQueueFamilyIndices
{
    bool is_complete()
    {
        return m_graphics_family.has_value() && m_present_family.has_value();
    }

    std::optional<u32> m_graphics_family;

    std::optional<u32> m_present_family;
};

struct VulkanSwapChainSupportDetails
{
    Internal::VkSurfaceCapabilitiesKHR m_capabilities;
    std::vector<Internal::VkSurfaceFormatKHR> m_formats;
    std::vector<Internal::VkPresentModeKHR> m_present_modes;
};
} // namespace Internal

class VulkanRenderer;


class VulkanWindow : public Window, public Singleton<VulkanWindow>
{
  public:
    static VulkanWindow &get()
    {
        static VulkanWindow instance;
        return instance;
    }

    void init() override;

    void pre_draw() override;

    void on_update() override;

    void post_draw() override;

    bool is_running() const override;

    void set_vsync(bool vsync) override;

    void set_window_title(const std::string &title) override;

    void draw() override;

    VulkanWindowData &get_window_data() const
    {
        return *m_window_data;
    }

    VulkanRenderer& get_renderer() const 
    {
        return *m_renderer;
    }

    static VKAPI_ATTR Internal::VkBool32 VKAPI_CALL
    debug_callback(Internal::VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                   Internal::VkDebugUtilsMessageTypeFlagsEXT message_type,
                   const Internal::VkDebugUtilsMessengerCallbackDataEXT *callback_data, void *user_data)
    {
        // Check if the message includes an error severity bit
        if (message_severity & Internal::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            SPARK_ERROR("[VULKAN] Validation layer: " << callback_data->pMessage);
        }
        else if (message_severity & Internal::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            SPARK_WARN("[VULKAN] Validation layer: " << callback_data->pMessage);
        }
        else if (message_severity & Internal::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                 Internal::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
        {
            SPARK_INFO("[VULKAN] Validation layer: " << callback_data->pMessage);
        }

        return VK_FALSE; // Indicate that the message has been handled
    }

    static void event_callback(std::shared_ptr<Event> event);

    static void framebuffer_resize_callback(Internal::GLFWwindow *window, i32 width, i32 hieght);

    static void close_event_callback(Internal::GLFWwindow *window);

    static void key_event_callback(Internal::GLFWwindow *window, i32 key, i32 scancode, i32 action, i32 mods);

    static void mouse_button_event_callback(Internal::GLFWwindow *window, i32 button, i32 action, i32 mods);

    static void mouse_scroll_event_callback(Internal::GLFWwindow *window, f64 xoffset, f64 yoffset);

    static void mouse_move_event_callback(Internal::GLFWwindow *window, f64 x, f64 y);

  private:
    VulkanWindow();

    ~VulkanWindow();

    void init_gl();

    void init_vulkan();

    void init_debug();

    void init_logical_device();

    void init_physical_device();

    void init_surface();

    void init_swap_chain();

    void init_image_views();

    void init_render_pass();

    void init_descriptor_set();

    void init_pipeline();

    void init_framebuffers();

    void init_command_pool();

    void init_command_buffers();

    void init_sync_objects();

    void init_descriptor_pool();

    void init_descriptor_sets();

    void reinit_swap_chain();

    bool check_validation_layer_support();

    std::vector<const char *> get_required_extensions();

    Internal::VkResult create_debug_utils_messenger_ext(Internal::VkInstance instance,
                                                        const Internal::VkDebugUtilsMessengerCreateInfoEXT *create_info,
                                                        const Internal::VkAllocationCallbacks *allocator,
                                                        Internal::VkDebugUtilsMessengerEXT *debug_messenger);

    void destroy_debug_utils_messenger_ext(Internal::VkInstance instance,
                                           Internal::VkDebugUtilsMessengerEXT debug_messenger,
                                           const Internal::VkAllocationCallbacks *allocator);

    void populate_debug_messenger(Internal::VkDebugUtilsMessengerCreateInfoEXT &create_info);

    void record_command_buffer(Internal::VkCommandBuffer command_buffer, u32 image_index);

    void draw_frame();

    void cleanup_swap_chain();

    bool is_device_suitable(Internal::VkPhysicalDevice device);

    bool check_device_extension_support(Internal::VkPhysicalDevice device);

    i32 rate_device(Internal::VkPhysicalDevice device);

    Internal::VulkanQueueFamilyIndices find_queue_families(Internal::VkPhysicalDevice device);

    Internal::VulkanSwapChainSupportDetails query_swap_chain_support(Internal::VkPhysicalDevice device);

    Internal::VkSurfaceFormatKHR choose_swap_surface_format(
        const std::vector<Internal::VkSurfaceFormatKHR> &available_formats);

    Internal::VkPresentModeKHR choose_swap_present_mode(
        const std::vector<Internal::VkPresentModeKHR> &available_present_modes);

    Internal::VkExtent2D choose_swap_extent(const Internal::VkSurfaceCapabilitiesKHR &capabilities);

    Internal::VkShaderModule create_shader_module(const std::vector<char> &code);

  private:
    Internal::GLFWwindow *m_window;

    std::unique_ptr<VulkanWindowData> m_window_data = std::make_unique<VulkanWindowData>();

    std::unique_ptr<VulkanRenderer> m_renderer = std::make_unique<VulkanRenderer>();

#ifdef DEBUG

    const bool m_enable_validation_layers = true;

#else

    const bool m_enable_validation_layers = false;

#endif
};
} // namespace Spark

#endif