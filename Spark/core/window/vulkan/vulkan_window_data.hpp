#ifndef SPARK_VULKAN_WINDOW_DATA_HPP
#define SPARK_VULKAN_WINDOW_DATA_HPP

#include "../../spark.hpp"
#include "../window_data.hpp"
#include "../../events/event.hpp"

namespace Spark
{
struct VulkanWindowData : public WindowData
{
    VulkanWindowData() : WindowData()
    {
    }

    VulkanWindowData(std::string title, bool vsync, i32 height, i32 width,
                     std::function<void(std::shared_ptr<Event>)> event_callback)
        : WindowData(title, vsync, height, width, event_callback)
    {
    }

    bool m_framebuffer_resized = false;

    u64 m_current_frame = 0;

    Internal::VkInstance m_instance;

    Internal::VkDebugUtilsMessengerEXT m_debug_messenger;

    Internal::VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;

    Internal::VkDevice m_device;

    Internal::VkQueue m_graphics_queue;

    Internal::VkQueue m_present_queue;

    Internal::VkSurfaceKHR m_surface;

    Internal::VkSwapchainKHR m_swapchain;

    Internal::VkFormat m_swapchain_image_format;

    Internal::VkExtent2D m_swapchain_extent;

    Internal::VkRenderPass m_render_pass;

    Internal::VkDescriptorSetLayout m_descriptor_set_layout;

    Internal::VkPipelineLayout m_pipeline_layout;

    Internal::VkPipeline m_graphics_pipeline;

    Internal::VkCommandPool m_command_pool;

    Internal::VkDescriptorPool m_descriptor_pool;

    std::vector<Internal::VkImage> m_swapchain_images;

    std::vector<Internal::VkImageView> m_swapchain_image_views;

    std::vector<Internal::VkFramebuffer> m_swapchain_framebuffers;

    std::vector<Internal::VkCommandBuffer> m_command_buffers;

    std::vector<Internal::VkSemaphore> m_image_available_semaphores;

    std::vector<Internal::VkSemaphore> m_render_finished_semaphores;

    std::vector<Internal::VkFence> m_in_flight_fences;

    std::vector<Internal::VkDescriptorSet> m_descriptor_sets;

    const i32 m_max_frames_in_flight = 2;

    const std::vector<const char *> m_validation_layers = {"VK_LAYER_KHRONOS_validation", "VK_LAYER_LUNARG_monitor"};

    const std::vector<const char *> m_device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};
}

#endif