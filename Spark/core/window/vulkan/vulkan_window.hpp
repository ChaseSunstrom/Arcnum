#ifndef SPARKvk_window_HPP
#define SPARKvk_window_HPP

#include "../window.hpp"
#include "../../util/singleton.hpp"

namespace spark
{
	struct VulkanQueueFamilyIndices
	{
		bool is_complete()
		{
			return m_graphics_family.has_value() && m_present_family.has_value();
		}

		std::optional <u32> m_graphics_family;

		std::optional <u32> m_present_family;
	};

	struct VulkanSwapChainSupportDetails
	{
		internal::VkSurfaceCapabilitiesKHR m_capabilities;
		std::vector <internal::VkSurfaceFormatKHR> m_formats;
		std::vector <internal::VkPresentModeKHR> m_present_modes;
	};

	struct VulkanWindowData :
			public WindowData
	{
		VulkanWindowData() :
				WindowData() { }

		VulkanWindowData(
				std::string title,
				bool vsync,
				i32 height,
				i32 width,
				std::function<void(std::shared_ptr < Event > )> event_callback) :
				WindowData(title, vsync, height, width, event_callback) { }

		bool m_framebuffer_resized = false;

		u64 m_current_frame = 0;

		internal::VkInstance m_instance;

		internal::VkDebugUtilsMessengerEXT m_debug_messenger;

		internal::VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;

		internal::VkDevice m_device;

		internal::VkQueue m_graphics_queue;

		internal::VkQueue m_present_queue;

		internal::VkSurfaceKHR m_surface;

		internal::VkSwapchainKHR m_swapchain;

		internal::VkFormat m_swapchain_image_format;

		internal::VkExtent2D m_swapchain_extent;

		internal::VkRenderPass m_render_pass;

		internal::VkDescriptorSetLayout m_descriptor_set_layout;

		internal::VkPipelineLayout m_pipeline_layout;

		internal::VkPipeline m_graphics_pipeline;

		internal::VkCommandPool m_command_pool;

		internal::VkDescriptorPool m_descriptor_pool;

		std::vector <internal::VkImage> m_swapchain_images;

		std::vector <internal::VkImageView> m_swapchain_image_views;

		std::vector <internal::VkFramebuffer> m_swapchain_framebuffers;

		std::vector <internal::VkCommandBuffer> m_command_buffers;

		std::vector <internal::VkSemaphore> m_image_available_semaphores;

		std::vector <internal::VkSemaphore> m_render_finished_semaphores;

		std::vector <internal::VkFence> m_in_flight_fences;

		std::vector <internal::VkDescriptorSet> m_descriptor_sets;

		const i32 m_max_frames_in_flight = 2;

		const std::vector<const char*> m_validation_layers = {
				"VK_LAYER_KHRONOS_validation",
				"VK_LAYER_LUNARG_monitor"
		};

		const std::vector<const char*> m_device_extensions = {
				VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
	};

	class VulkanWindow :
			public Window, public Singleton<VulkanWindow>
	{
	public:
		static VulkanWindow& get()
		{
			static VulkanWindow instance;
			return instance;
		}

		void pre_draw() override;

		void on_update() override;

		void post_draw() override;

		bool is_running() override;

		void set_vsync(bool vsync) override;

		void set_window_title(const std::string& title) override;

		VulkanWindowData& get_window_data() const
		{
			return *m_window_data;
		}

		static VKAPI_ATTR internal::VkBool32 VKAPI_CALL debug_callback(
			internal::VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
			internal::VkDebugUtilsMessageTypeFlagsEXT message_type,
				const internal::VkDebugUtilsMessengerCallbackDataEXT* callback_data,
				void* user_data)
		{
			// Check if the message includes an error severity bit
			if (message_severity & internal::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			{
				SPARK_ERROR("[VULKAN] Validation layer: " << callback_data->pMessage);
			}
			else if (message_severity & internal::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			{
				SPARK_WARN("[VULKAN] Validation layer: " << callback_data->pMessage);
			}
			else if (message_severity & internal::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
				internal::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
			{
				SPARK_INFO("[VULKAN] Validation layer: " << callback_data->pMessage);
			}

			return VK_FALSE; // Indicate that the message has been handled
		}

		static void event_callback(std::shared_ptr <Event> event);

		static void framebuffer_resize_callback(internal::GLFWwindow* window, i32 width, i32 hieght);

		static void close_event_callback(internal::GLFWwindow* window);

		static void key_event_callback(internal::GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods);

		static void mouse_button_event_callback(internal::GLFWwindow* window, i32 button, i32 action, i32 mods);

		static void mouse_scroll_event_callback(internal::GLFWwindow* window, f64 xoffset, f64 yoffset);

		static void mouse_move_event_callback(internal::GLFWwindow* window, f64 x, f64 y);

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

		std::vector<const char*> get_required_extensions();

		internal::VkResult create_debug_utils_messenger_ext(
			internal::VkInstance instance,
				const internal::VkDebugUtilsMessengerCreateInfoEXT* create_info,
				const internal::VkAllocationCallbacks* allocator,
			internal::VkDebugUtilsMessengerEXT* debug_messenger);

		void destroy_debug_utils_messenger_ext(
			internal::VkInstance instance,
			internal::VkDebugUtilsMessengerEXT debug_messenger,
				const internal::VkAllocationCallbacks* allocator);

		void populate_debug_messenger(internal::VkDebugUtilsMessengerCreateInfoEXT& create_info);

		void record_command_buffer(internal::VkCommandBuffer command_buffer, u32 image_index);

		void draw_frame();

		void cleanup_swap_chain();

		bool is_device_suitable(internal::VkPhysicalDevice device);

		bool check_device_extension_support(internal::VkPhysicalDevice device);

		i32 rate_device(internal::VkPhysicalDevice device);

		VulkanQueueFamilyIndices find_queue_families(internal::VkPhysicalDevice device);

		VulkanSwapChainSupportDetails query_swap_chain_support(internal::VkPhysicalDevice device);

		internal::VkSurfaceFormatKHR choose_swap_surface_format(const std::vector <internal::VkSurfaceFormatKHR>& available_formats);

		internal::VkPresentModeKHR choose_swap_present_mode(const std::vector <internal::VkPresentModeKHR>& available_present_modes);

		internal::VkExtent2D choose_swap_extent(const internal::VkSurfaceCapabilitiesKHR& capabilities);

		internal::VkShaderModule create_shader_module(const std::vector<char>& code);

	private:
		internal::GLFWwindow* m_window;

		std::unique_ptr <VulkanWindowData> m_window_data = std::make_unique<VulkanWindowData>();

#ifdef DEBUG

		const bool m_enable_validation_layers = true;

#else

		const bool m_enable_validation_layers = false;

#endif
	};
}

#endif