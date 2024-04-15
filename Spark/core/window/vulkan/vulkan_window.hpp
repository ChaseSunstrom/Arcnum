#ifndef SPARKvk_window_HPP
#define SPARKvk_window_HPP

#include "../window.hpp"

namespace spark
{
	struct queue_family_indices
	{
		bool is_complete()
		{
			return m_graphics_family.has_value() && m_present_family.has_value();
		}

		std::optional <u32> m_graphics_family;

		std::optional <u32> m_present_family;
	};

	struct swap_chain_support_details
	{
		VkSurfaceCapabilitiesKHR m_capabilities;
		std::vector <VkSurfaceFormatKHR> m_formats;
		std::vector <VkPresentModeKHR> m_present_modes;
	};

	struct vulkan_window_data :
			public window_data
	{
		vulkan_window_data() :
				window_data() { }

		vulkan_window_data(
				std::string title,
				bool vsync,
				i32 height,
				i32 width,
				std::function<void(std::shared_ptr < event > )> event_callback) :
				window_data(title, vsync, height, width, event_callback) { }

		bool m_framebuffer_resized = false;

		u64 m_current_frame = 0;

		VkInstance m_instance;

		VkDebugUtilsMessengerEXT m_debug_messenger;

		VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;

		VkDevice m_device;

		VkQueue m_graphics_queue;

		VkQueue m_present_queue;

		VkSurfaceKHR m_surface;

		VkSwapchainKHR m_swapchain;

		VkFormat m_swapchain_image_format;

		VkExtent2D m_swapchain_extent;

		VkRenderPass m_render_pass;

		VkDescriptorSetLayout m_descriptor_set_layout;

		VkPipelineLayout m_pipeline_layout;

		VkPipeline m_graphics_pipeline;

		VkCommandPool m_command_pool;

		VkDescriptorPool m_descriptor_pool;

		std::vector <VkImage> m_swapchain_images;

		std::vector <VkImageView> m_swapchain_image_views;

		std::vector <VkFramebuffer> m_swapchain_framebuffers;

		std::vector <VkCommandBuffer> m_command_buffers;

		std::vector <VkSemaphore> m_image_available_semaphores;

		std::vector <VkSemaphore> m_render_finished_semaphores;

		std::vector <VkFence> m_in_flight_fences;

		std::vector <VkDescriptorSet> m_descriptor_sets;

		const i32 m_max_frames_in_flight = 2;

		const std::vector<const char*> m_validation_layers = {
				"VK_LAYER_KHRONOS_validation"
		};

		const std::vector<const char*> m_device_extensions = {
				VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
	};

	class vulkan_window :
			public window
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

		vulkan_window_data& get_window_data()
		{
			return *m_window_data;
		}

		static VKAPI_ATTR VkBool32

		VKAPI_CALL debug_callback(
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
			else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
			         VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
			{
				SPARK_INFO("[VULKAN] Validation layer: " << callback_data->pMessage);
			}

			return VK_FALSE; // Indicate that the message has been handled
		}

		static void event_callback(std::shared_ptr <event> event);

		static void framebuffer_resize_callback(GLFWwindow* window, i32 width, i32 hieght);

		static void close_event_callback(GLFWwindow* window);

		static void key_event_callback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods);

		static void mouse_button_event_callback(GLFWwindow* window, i32 button, i32 action, i32 mods);

		static void mouse_scroll_event_callback(GLFWwindow* window, f64 xoffset, f64 yoffset);

		static void mouse_move_event_callback(GLFWwindow* window, f64 x, f64 y);

	private:
		vulkan_window();

		~vulkan_window();

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

		void init_imgui();

		void reinit_swap_chain();

		bool check_validation_layer_support();

		std::vector<const char*> get_required_extensions();

		VkResult create_debug_utils_messenger_ext(
				VkInstance instance,
				const VkDebugUtilsMessengerCreateInfoEXT* create_info,
				const VkAllocationCallbacks* allocator,
				VkDebugUtilsMessengerEXT* debug_messenger);

		void destroy_debug_utils_messenger_ext(
				VkInstance instance,
				VkDebugUtilsMessengerEXT debug_messenger,
				const VkAllocationCallbacks* allocator);

		void populate_debug_messenger(VkDebugUtilsMessengerCreateInfoEXT& create_info);

		void record_command_buffer(VkCommandBuffer command_buffer, u32 image_index);

		void draw_frame();

		void cleanup_swap_chain();

		bool is_device_suitable(VkPhysicalDevice device);

		bool check_device_extension_support(VkPhysicalDevice device);

		i32 rate_device(VkPhysicalDevice device);

		queue_family_indices find_queue_families(VkPhysicalDevice device);

		swap_chain_support_details query_swap_chain_support(VkPhysicalDevice device);

		VkSurfaceFormatKHR choose_swap_surface_format(const std::vector <VkSurfaceFormatKHR>& available_formats);

		VkPresentModeKHR choose_swap_present_mode(const std::vector <VkPresentModeKHR>& available_present_modes);

		VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);

		VkShaderModule create_shader_module(const std::vector<char>& code);

	private:
		GLFWwindow* m_window;

		std::unique_ptr <vulkan_window_data> m_window_data = std::make_unique<vulkan_window_data>();

#ifdef DEBUG
		const bool m_enable_validation_layers = true;
#else

		const bool m_enable_validation_layers = false;

#endif
	};
}

#endif