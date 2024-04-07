#ifndef SPARK_VULKAN_WINDOW_HPP
#define SPARK_VULKAN_WINDOW_HPP

#include "window.hpp"

namespace spark
{
	struct queue_family_indices
	{
		bool is_complete()
		{
			return m_graphics_family.has_value() && m_present_family.has_value();
		}

		std::optional<uint32_t> m_graphics_family;
		std::optional<uint32_t> m_present_family;
	};
	
	struct vulkan_window_data : public window_data
	{
		vulkan_window_data() : window_data() {}

		vulkan_window_data(std::string title,
			bool vsync,
			int32_t height,
			int32_t width,
			std::function<void(std::shared_ptr<event>)> event_callback) :
			window_data(title, vsync, height, width, event_callback) {}

		VkInstance m_instance;
		VkDebugUtilsMessengerEXT m_debug_messenger;
		VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
		VkDevice m_device;
		VkQueue m_graphics_queue;
		VkQueue m_present_queue;
		VkSurfaceKHR m_surface;
	};

	class vulkan_window : public window
	{
	public:
		static vulkan_window& get()
		{
			static vulkan_window instance;
			return instance;
		}

		void pre_draw() override;

		void on_update() override;

		void post_draw() override;

		bool is_running() override;

		void set_vsync(bool vsync) override;

		void set_window_title(const std::string& title) override;

		vulkan_window_data& get_window_data() {
			return *m_window_data;
		}
		static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
			VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
			VkDebugUtilsMessageTypeFlagsEXT message_type,
			const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
			void* user_data)
		{
			// Check if the message includes an error severity bit
			if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			{
				SPARK_ERROR("[VULKAN] Validation layer: " << callback_data->pMessage);
			}
			else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			{
				SPARK_WARN("[VULKAN] Validation layer: " << callback_data->pMessage);
			}
			else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
			{
				SPARK_INFO("[VULKAN] Validation layer: " << callback_data->pMessage);
			}

			return VK_FALSE; // Indicate that the message has been handled
		}

	private:
		vulkan_window();
		~vulkan_window();
		void init_gl();
		void init_vulkan();
		void init_debug();

		bool check_validation_layer_support();
		std::vector<const char*> get_required_extensions();
		VkResult create_debug_utils_messenger_ext(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* create_info, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debug_messenger);
		void destroy_debug_utils_messenger_ext(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* allocator);
		void populate_debug_messenger(VkDebugUtilsMessengerCreateInfoEXT& create_info);
		void create_logical_device();
		void pick_physical_device();
		void create_surface();
		bool is_device_suitable(VkPhysicalDevice device);
		int32_t rate_device(VkPhysicalDevice device);
		queue_family_indices find_queue_families(VkPhysicalDevice device);
	private:
		GLFWwindow* m_window;

		std::unique_ptr<vulkan_window_data> m_window_data = std::make_unique<vulkan_window_data>();

		const std::vector<const char*> m_validation_layers = {
			"VK_LAYER_KHRONOS_validation"
		};

#ifdef DEBUG
		const bool m_enable_validation_layers = true;
#else
		const bool m_enable_validation_layers = false;
#endif
	};
}

#endif