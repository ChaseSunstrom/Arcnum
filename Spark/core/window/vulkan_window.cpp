#include "vulkan_window.hpp"
#include "opengl_window.hpp"

#include "../util/file.hpp"
#include "../events/sub.hpp"

#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_glfw.h"
#include "IMGUI/imgui_impl_vulkan.h"

namespace spark
{
	vulkan_window::vulkan_window() :
		window(window_type::VULKAN)
	{
		m_window_data = std::make_unique<vulkan_window_data>("Title", false, 1080, 1080, event_callback);

		init_gl();
		init_vulkan();
		init_debug();
		init_surface();
		init_physical_device();
		init_logical_device();
		init_swap_chain();
		init_image_views();
		init_render_pass();
		init_pipeline();
		init_framebuffers();
		init_command_pool();
		init_command_buffers();
		init_sync_objects();
		// Uncomment this when done fully implementing vulkan
		//init_imgui();
	}

	vulkan_window::~vulkan_window()
	{
		cleanup_swap_chain();

		for (auto& framebuffer : m_window_data->m_swapchain_framebuffers)
		{
			vkDestroyFramebuffer(m_window_data->m_device, framebuffer, nullptr);
		}

		vkDestroyPipeline(m_window_data->m_device, m_window_data->m_graphics_pipeline, nullptr);
		vkDestroyPipelineLayout(m_window_data->m_device, m_window_data->m_pipeline_layout, nullptr);
		vkDestroyRenderPass(m_window_data->m_device, m_window_data->m_render_pass, nullptr);

		for (uint64_t i = 0; i < m_window_data->m_max_frames_in_flight; i++)
		{
			vkDestroySemaphore(m_window_data->m_device, m_window_data->m_image_available_semaphores[i], nullptr);
			vkDestroySemaphore(m_window_data->m_device, m_window_data->m_render_finished_semaphores[i], nullptr);
			vkDestroyFence(m_window_data->m_device, m_window_data->m_in_flight_fences[i], nullptr);
		}

		vkDestroyCommandPool(m_window_data->m_device, m_window_data->m_command_pool, nullptr);

		for (auto & import : m_window_data->m_swapchain_image_views)
		{
			vkDestroyImageView(m_window_data->m_device, import, nullptr);
		}

		vkDestroySwapchainKHR(m_window_data->m_device, m_window_data->m_swapchain, nullptr);

		if (m_enable_validation_layers)
		{
			destroy_debug_utils_messenger_ext(m_window_data->m_instance, m_window_data->m_debug_messenger, nullptr);
		}

		vkDestroySurfaceKHR(m_window_data->m_instance, m_window_data->m_surface, nullptr);
		vkDestroyInstance(m_window_data->m_instance, nullptr);
		vkDestroyDevice(m_window_data->m_device, nullptr);

		glfwDestroyWindow(m_window);

		glfwTerminate();
	}

	bool vulkan_window::check_validation_layer_support()
	{
		uint32_t layer_count;

		vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

		std::vector<VkLayerProperties> available_layers(layer_count);
		vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

		for (const char* layer_name : m_window_data->m_validation_layers)
		{
			bool layer_found = false;

			for (const auto& layer_properties : available_layers)
			{
				if (std::strcmp(layer_name, layer_properties.layerName) == 0)
				{
					layer_found = true;
					break;
				}
			}

			if (!layer_found)
				return false;
		}

		return true;
	}

	void vulkan_window::draw_frame()
	{
		vkWaitForFences(m_window_data->m_device, 1, &m_window_data->m_in_flight_fences[m_window_data->m_current_frame], VK_TRUE, UINT64_MAX);
		
		uint32_t image_index;
		VkResult result =vkAcquireNextImageKHR(m_window_data->m_device, m_window_data->m_swapchain, UINT64_MAX, m_window_data->m_image_available_semaphores[m_window_data->m_current_frame], VK_NULL_HANDLE, &image_index);
		
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			reinit_swap_chain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			SPARK_ERROR("[VULKAN] Failed to acquire swap chain image!");
			assert(false);
		}
		
		vkResetFences(m_window_data->m_device, 1, &m_window_data->m_in_flight_fences[m_window_data->m_current_frame]);
		vkResetCommandBuffer(m_window_data->m_command_buffers[m_window_data->m_current_frame], 0);

		record_command_buffer(m_window_data->m_command_buffers[m_window_data->m_current_frame], image_index);

		VkSubmitInfo submit_info{};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore wait_semaphores[] = { m_window_data->m_image_available_semaphores[m_window_data->m_current_frame] };
		VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = wait_semaphores;
		submit_info.pWaitDstStageMask = wait_stages;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &m_window_data->m_command_buffers[m_window_data->m_current_frame];

		VkSemaphore signal_semaphores[] = { m_window_data->m_render_finished_semaphores[m_window_data->m_current_frame] };
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = signal_semaphores;

		if (vkQueueSubmit(m_window_data->m_graphics_queue, 1, &submit_info, m_window_data->m_in_flight_fences[m_window_data->m_current_frame]) != VK_SUCCESS)
		{
			SPARK_ERROR("[VULKAN] Failed to submit draw command buffer!");
			assert(false);
		}

		VkPresentInfoKHR present_info{};
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = signal_semaphores;

		VkSwapchainKHR swapchains[] = { m_window_data->m_swapchain };
		present_info.swapchainCount = 1;
		present_info.pSwapchains = swapchains;
		present_info.pImageIndices = &image_index;
		present_info.pResults = nullptr;

		result = vkQueuePresentKHR(m_window_data->m_present_queue, &present_info);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_window_data->m_framebuffer_resized)
		{
			m_window_data->m_framebuffer_resized = false;
			reinit_swap_chain();
		}
		else if (result != VK_SUCCESS)
		{
			SPARK_ERROR("[VULKAN] Failed to present swap chain image!");
			assert(false);
		}

		m_window_data->m_current_frame = (m_window_data->m_current_frame + 1) % m_window_data->m_max_frames_in_flight;
	}

	void vulkan_window::pre_draw()
	{
		glfwPollEvents();
	}

	void vulkan_window::on_update()
	{
		draw_frame();
	}

	void vulkan_window::post_draw()
	{}

	bool vulkan_window::is_running()
	{
		return !glfwWindowShouldClose(m_window);
	}

	void vulkan_window::set_vsync(bool vsync)
	{}

	void vulkan_window::set_window_title(const std::string& title)
	{
		glfwSetWindowTitle(m_window, title.c_str());
	}

	std::vector<const char*> vulkan_window::get_required_extensions()
	{
		uint32_t glfw_extension_count = 0;
		const char** glfw_extensions;

		glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

		std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

		if (m_enable_validation_layers)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		return extensions;
	}

	VkResult vulkan_window::create_debug_utils_messenger_ext(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* create_info, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debug_messenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			return func(instance, create_info, allocator, debug_messenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void vulkan_window::destroy_debug_utils_messenger_ext(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* allocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			func(instance, debug_messenger, allocator);
		}
	}

	void vulkan_window::init_physical_device()
	{
		uint32_t device_count = 0;
		vkEnumeratePhysicalDevices(m_window_data->m_instance, &device_count, nullptr);

		if (device_count == 0)
		{
			SPARK_ERROR("[VULKAN] Failed to find GPUs with Vulkan support!");
			assert(false);
		}

		std::vector<VkPhysicalDevice> devices(device_count);
		vkEnumeratePhysicalDevices(m_window_data->m_instance, &device_count, devices.data());

		std::multimap<int32_t, VkPhysicalDevice> candidates;

		for (const auto& device : devices)
		{
			int32_t score = rate_device(device);
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(device, &properties);

			SPARK_INFO("[VULKAN] Device " + std::string(properties.deviceName) + " was given score: " + std::to_string(score));

			candidates.insert(std::make_pair(score, device));
		}


		if (candidates.rbegin()->first > 0)
		{
			m_window_data->m_physical_device = candidates.rbegin()->second;
		}
		else
		{
			SPARK_ERROR("[VULKAN] Failed to find suitable GPU!");
			assert(false);
		}
	}

	queue_family_indices vulkan_window::find_queue_families(VkPhysicalDevice device)
	{
		queue_family_indices indices;

		uint32_t queue_family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

		std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

		uint32_t i = 0;
		for (const auto& queue_family : queue_families)
		{
			if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.m_graphics_family = i;

			VkBool32 present_support = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_window_data->m_surface, &present_support);

			if (present_support)
				indices.m_present_family = i;

			if (indices.is_complete())
				break;

			i++;
		}

		return indices;
	}

	bool vulkan_window::is_device_suitable(VkPhysicalDevice device)
	{
		queue_family_indices indices = find_queue_families(device);

		bool extensions_supported = check_device_extension_support(device);

		bool swap_chain_adequate = false;
		if (extensions_supported)
		{
			swap_chain_support_details swap_chain_support = query_swap_chain_support(device);
			swap_chain_adequate = !swap_chain_support.m_formats.empty() && !swap_chain_support.m_present_modes.empty();
		}

		return indices.is_complete() && extensions_supported && swap_chain_adequate;
	}

	bool vulkan_window::check_device_extension_support(VkPhysicalDevice device)
	{
		uint32_t extension_count;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

		std::vector<VkExtensionProperties> available_extensions(extension_count);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

		std::set<std::string> required_extensions(m_window_data->m_device_extensions.begin(), m_window_data->m_device_extensions.end());

		for (const auto& extension : available_extensions)
		{
			required_extensions.erase(extension.extensionName);
		}

		return required_extensions.empty();
	}

	int32_t vulkan_window::rate_device(VkPhysicalDevice device)
	{
		VkPhysicalDeviceProperties device_properties;
		VkPhysicalDeviceFeatures device_features;
		VkPhysicalDeviceMemoryProperties memory_properties;

		vkGetPhysicalDeviceProperties(device, &device_properties);
		vkGetPhysicalDeviceFeatures(device, &device_features);
		vkGetPhysicalDeviceMemoryProperties(device, &memory_properties);

		int32_t score = 0;

		// Device type preference: Discrete GPU is given a high score
		if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			score += 1000;
		}
		else if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
		{
			score += 500; // Less preferable, but still viable
		}

		// Feature support
		if (device_features.geometryShader)
		{
			score += 100;
		}
		if (device_features.tessellationShader)
		{
			score += 100;
		}

		// Maximum texture dimension 2D for texture-heavy applications
		score += device_properties.limits.maxImageDimension2D;

		// Assess the memory capacity
		uint64_t total_memory = 0;
		for (uint32_t i = 0; i < memory_properties.memoryHeapCount; i++)
		{
			if (memory_properties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
			{
				total_memory += memory_properties.memoryHeaps[i].size;
			}
		}
		score += static_cast<int32_t>(total_memory >> 20); // Add one point per MB of GPU memory

		// Ensure the device supports all required queue families
		queue_family_indices indices = find_queue_families(device);
		if (!indices.is_complete())
		{
			score = 0; // Disqualify the device if it does not meet the queue family requirements
		}

		return score;
	}

	void vulkan_window::cleanup_swap_chain()
	{
		for (uint64_t i = 0; i < m_window_data->m_swapchain_framebuffers.size(); i++)
		{
			vkDestroyFramebuffer(m_window_data->m_device, m_window_data->m_swapchain_framebuffers[i], nullptr);
		}

		for (uint64_t i = 0; i < m_window_data->m_swapchain_image_views.size(); i++)
		{
			vkDestroyImageView(m_window_data->m_device, m_window_data->m_swapchain_image_views[i], nullptr);
		}

		vkDestroySwapchainKHR(m_window_data->m_device, m_window_data->m_swapchain, nullptr);
	}

	void vulkan_window::reinit_swap_chain()
	{
		int32_t width = 0;
		int32_t height = 0;

		glfwGetFramebufferSize(m_window, &width, &height);

		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(m_window, &width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(m_window_data->m_device);

		cleanup_swap_chain();

		init_swap_chain();
		init_image_views();
		init_framebuffers();
	}

	void vulkan_window::populate_debug_messenger(VkDebugUtilsMessengerCreateInfoEXT& create_info)
	{
		create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		create_info.pfnUserCallback = debug_callback;
	}

	void vulkan_window::init_sync_objects()
	{
		m_window_data->m_image_available_semaphores.resize(m_window_data->m_max_frames_in_flight);
		m_window_data->m_render_finished_semaphores.resize(m_window_data->m_max_frames_in_flight);
		m_window_data->m_in_flight_fences.resize(m_window_data->m_max_frames_in_flight);

		VkSemaphoreCreateInfo semaphore_info{};
		semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fence_info{};
		fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (uint64_t i = 0; i < m_window_data->m_max_frames_in_flight; i++)
		{
			if (vkCreateSemaphore(m_window_data->m_device, &semaphore_info, nullptr, &m_window_data->m_image_available_semaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(m_window_data->m_device, &semaphore_info, nullptr, &m_window_data->m_render_finished_semaphores[i]) != VK_SUCCESS ||
				vkCreateFence(m_window_data->m_device, &fence_info, nullptr, &m_window_data->m_in_flight_fences[i]) != VK_SUCCESS)
			{
				SPARK_ERROR("[VULKAN] Failed to create semaphore!");
				assert(false);
			}
		}
	}

	void vulkan_window::init_gl()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_window = glfwCreateWindow(
			m_window_data->m_width, m_window_data->m_height, m_window_data->m_title.c_str(), NULL, NULL);

		glfwSetWindowUserPointer(m_window, m_window_data.get());

		glfwSetFramebufferSizeCallback(m_window, framebuffer_resize_callback);
		glfwSetWindowSizeCallback(m_window, opengl_window::resized_event_callback);
		glfwSetWindowCloseCallback(m_window, opengl_window::close_event_callback);
		glfwSetKeyCallback(m_window, opengl_window::key_event_callback);
		glfwSetMouseButtonCallback(m_window, opengl_window::mouse_button_event_callback);
		glfwSetCursorPosCallback(m_window, opengl_window::mouse_move_event_callback);
		glfwSetScrollCallback(m_window, opengl_window::mouse_scroll_event_callback);
	}

	void vulkan_window::init_vulkan()
	{
		VkApplicationInfo app_info = {};
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.pApplicationName = m_window_data->m_title.c_str();
		app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		app_info.pEngineName = "Spark";
		app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		app_info.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.pApplicationInfo = &app_info;

		auto extensions = get_required_extensions();
		create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		create_info.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};

		if (m_enable_validation_layers)
		{
			create_info.enabledLayerCount = static_cast<uint32_t>(m_window_data->m_validation_layers.size());
			create_info.ppEnabledLayerNames = m_window_data->m_validation_layers.data();

			populate_debug_messenger(debug_create_info);
			create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
		}
		else
		{
			create_info.enabledLayerCount = 0;
			create_info.pNext = nullptr;
		}

		VkResult result = vkCreateInstance(&create_info, nullptr, &m_window_data->m_instance);
	}

	swap_chain_support_details vulkan_window::query_swap_chain_support(VkPhysicalDevice device)
	{
		swap_chain_support_details details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_window_data->m_surface, &details.m_capabilities);

		uint32_t format_count;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_window_data->m_surface, &format_count, nullptr);

		if (format_count != 0)
		{
			details.m_formats.resize(format_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_window_data->m_surface, &format_count, details.m_formats.data());
		}

		uint32_t present_mode_count;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_window_data->m_surface, &present_mode_count, nullptr);

		if (present_mode_count != 0)
		{
			details.m_present_modes.resize(present_mode_count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_window_data->m_surface, &present_mode_count, details.m_present_modes.data());
		}

		return details;
	}

	void vulkan_window::init_swap_chain()
	{
		swap_chain_support_details swap_chain_support = query_swap_chain_support(m_window_data->m_physical_device);

		VkSurfaceFormatKHR surface_format = choose_swap_surface_format(swap_chain_support.m_formats);
		VkPresentModeKHR present_mode = choose_swap_present_mode(swap_chain_support.m_present_modes);
		VkExtent2D extent = choose_swap_extent(swap_chain_support.m_capabilities);

		uint32_t image_count = swap_chain_support.m_capabilities.minImageCount + 1;
		if (swap_chain_support.m_capabilities.maxImageCount > 0 && image_count > swap_chain_support.m_capabilities.maxImageCount)
		{
			image_count = swap_chain_support.m_capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.surface = m_window_data->m_surface;
		create_info.minImageCount = image_count;
		create_info.imageFormat = surface_format.format;
		create_info.imageColorSpace = surface_format.colorSpace;
		create_info.imageExtent = extent;
		create_info.imageArrayLayers = 1;
		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		queue_family_indices indices = find_queue_families(m_window_data->m_physical_device);
		uint32_t queue_family_indices[] = { indices.m_graphics_family.value(), indices.m_present_family.value() };

		if (indices.m_graphics_family != indices.m_present_family)
		{
			create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			create_info.queueFamilyIndexCount = 2;
			create_info.pQueueFamilyIndices = queue_family_indices;
		}
		else
		{
			create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		create_info.preTransform = swap_chain_support.m_capabilities.currentTransform;
		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		create_info.presentMode = present_mode;
		create_info.clipped = VK_TRUE;
		create_info.oldSwapchain = VK_NULL_HANDLE;

		VkResult result = vkCreateSwapchainKHR(m_window_data->m_device, &create_info, nullptr, &m_window_data->m_swapchain);
		
		if (result != VK_SUCCESS)
		{
			SPARK_ERROR("[VULKAN] Failed to create swap chain!");
			assert(false);
		}

		vkGetSwapchainImagesKHR(m_window_data->m_device, m_window_data->m_swapchain, &image_count, nullptr);
		m_window_data->m_swapchain_images.resize(image_count);
		vkGetSwapchainImagesKHR(m_window_data->m_device, m_window_data->m_swapchain, &image_count, m_window_data->m_swapchain_images.data());

		m_window_data->m_swapchain_image_format = surface_format.format;
		m_window_data->m_swapchain_extent = extent;
	}


	void vulkan_window::init_logical_device()
	{
		queue_family_indices indices = find_queue_families(m_window_data->m_physical_device);

		std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
		std::set<uint32_t> unique_queue_families = { indices.m_graphics_family.value(), indices.m_present_family.value() };

		float32_t queue_priority = 1.0f;

		for (uint32_t queue_family : unique_queue_families)
		{
			VkDeviceQueueCreateInfo queue_create_info{};
			queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_create_info.queueFamilyIndex = queue_family;
			queue_create_info.queueCount = 1;
			queue_create_info.pQueuePriorities = &queue_priority;
			queue_create_infos.push_back(queue_create_info);
		}

		VkPhysicalDeviceFeatures device_features{};
		VkDeviceCreateInfo create_info{};

		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
		create_info.pQueueCreateInfos = queue_create_infos.data();
		create_info.pEnabledFeatures = &device_features;

		create_info.enabledExtensionCount = static_cast<uint32_t>(m_window_data->m_device_extensions.size());
		create_info.ppEnabledExtensionNames = m_window_data->m_device_extensions.data();

		if (m_enable_validation_layers)
		{
			create_info.enabledLayerCount = static_cast<uint32_t>(m_window_data->m_validation_layers.size());
			create_info.ppEnabledLayerNames = m_window_data->m_validation_layers.data();
		}
		else
		{
			create_info.enabledLayerCount = 0;
		}
		if (vkCreateDevice(m_window_data->m_physical_device, &create_info, nullptr, &m_window_data->m_device) != VK_SUCCESS)
		{
			SPARK_ERROR("[VULKAN] Failed to create logical device!");
			assert(false);
		}

		vkGetDeviceQueue(m_window_data->m_device, indices.m_graphics_family.value(), 0, &m_window_data->m_graphics_queue);
		vkGetDeviceQueue(m_window_data->m_device, indices.m_present_family.value(), 0, &m_window_data->m_present_queue);
	}

	void vulkan_window::init_pipeline()
	{
		std::vector<char> vertex_shader_code = read_file_to_bytes("Spark/shaders/default_vert.spv");
		std::vector<char> fragment_shader_code = read_file_to_bytes("Spark/shaders/default_frag.spv");

		VkShaderModule vertex_shader_module = create_shader_module(vertex_shader_code);
		VkShaderModule fragment_shader_module = create_shader_module(fragment_shader_code);

		VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
		vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vert_shader_stage_info.module = vertex_shader_module;
		vert_shader_stage_info.pName = "main";

		VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
		frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		frag_shader_stage_info.module = fragment_shader_module;
		frag_shader_stage_info.pName = "main";

		VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_stage_info, frag_shader_stage_info };

		VkPipelineVertexInputStateCreateInfo vertex_input_info{};
		vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertex_input_info.vertexBindingDescriptionCount = 0; 
		vertex_input_info.vertexAttributeDescriptionCount = 0;

		VkPipelineInputAssemblyStateCreateInfo input_assembly{};
		input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		input_assembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewport_state{};
		viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_state.viewportCount = 1;
		viewport_state.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo color_blending{};
		color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		color_blending.logicOpEnable = VK_FALSE;
		color_blending.logicOp = VK_LOGIC_OP_COPY;
		color_blending.attachmentCount = 1;
		color_blending.pAttachments = &colorBlendAttachment;
		color_blending.blendConstants[0] = 0.0f;
		color_blending.blendConstants[1] = 0.0f;
		color_blending.blendConstants[2] = 0.0f;
		color_blending.blendConstants[3] = 0.0f;

		std::vector<VkDynamicState> dynamic_states = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamic_state{};
		dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
		dynamic_state.pDynamicStates = dynamic_states.data();

		VkPipelineLayoutCreateInfo pipeline_layout_info{};
		pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_info.setLayoutCount = 0;
		pipeline_layout_info.pushConstantRangeCount = 0;

		if (vkCreatePipelineLayout(m_window_data->m_device, &pipeline_layout_info, nullptr, &m_window_data->m_pipeline_layout) != VK_SUCCESS) 
		{
			SPARK_ERROR("[VULKAN] Failed to create pipeline layout!");
			assert(false);
		}

		VkGraphicsPipelineCreateInfo pipeline_info{};
		pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_info.stageCount = 2;
		pipeline_info.pStages = shader_stages;
		pipeline_info.pVertexInputState = &vertex_input_info;
		pipeline_info.pInputAssemblyState = &input_assembly;
		pipeline_info.pViewportState = &viewport_state;
		pipeline_info.pRasterizationState = &rasterizer;
		pipeline_info.pMultisampleState = &multisampling;
		pipeline_info.pDepthStencilState = nullptr;
		pipeline_info.pColorBlendState = &color_blending;
		pipeline_info.pDynamicState = &dynamic_state;
		pipeline_info.layout = m_window_data->m_pipeline_layout;
		pipeline_info.renderPass = m_window_data->m_render_pass;
		pipeline_info.subpass = 0;
		pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
		pipeline_info.basePipelineIndex = -1;

		if (vkCreateGraphicsPipelines(m_window_data->m_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &m_window_data->m_graphics_pipeline) != VK_SUCCESS)
		{
			SPARK_ERROR("[VULKAN] Failed to create graphics pipeline!");
			assert(false);
		}

		vkDestroyShaderModule(m_window_data->m_device, fragment_shader_module, nullptr);
		vkDestroyShaderModule(m_window_data->m_device, vertex_shader_module, nullptr);
	}

	VkShaderModule vulkan_window::create_shader_module(const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.codeSize = code.size();
		create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shader_module;
		if (vkCreateShaderModule(m_window_data->m_device, &create_info, nullptr, &shader_module) != VK_SUCCESS)
		{
			SPARK_ERROR("[VULKAN] Failed to create shader module!");
			assert(false);
		}

		return shader_module;
	}

	VkSurfaceFormatKHR vulkan_window::choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats)
	{
		for (const auto& available_format : available_formats)
		{
			if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return available_format;
			}
		}

		return available_formats[0];
	}

	VkPresentModeKHR vulkan_window::choose_swap_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes)
	{
		for (const auto& available_present_mode : available_present_modes)
		{
			if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return available_present_mode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D vulkan_window::choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}
		else
		{
			int32_t width, height;
			glfwGetFramebufferSize(m_window, &width, &height);

			VkExtent2D actual_extent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actual_extent;
		}
	}

	void vulkan_window::init_surface()
	{
		if (glfwCreateWindowSurface(m_window_data->m_instance, m_window, nullptr, &m_window_data->m_surface) != VK_SUCCESS)
		{
			SPARK_ERROR("[VULKAN] Failed to create window surface!");
			assert(false);
		}
	}

	void vulkan_window::init_image_views()
	{
		m_window_data->m_swapchain_image_views.resize(m_window_data->m_swapchain_images.size());

		for (uint64_t i = 0; i < m_window_data->m_swapchain_images.size(); i++)
		{
			VkImageViewCreateInfo create_info{};
			create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			create_info.image = m_window_data->m_swapchain_images[i];
			create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			create_info.format = m_window_data->m_swapchain_image_format;

			create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			create_info.subresourceRange.baseMipLevel = 0;
			create_info.subresourceRange.levelCount = 1;
			create_info.subresourceRange.baseArrayLayer = 0;
			create_info.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_window_data->m_device, &create_info, nullptr, &m_window_data->m_swapchain_image_views[i]) != VK_SUCCESS)
			{
				SPARK_ERROR("[VULKAN] Failed to create image views!");
				assert(false);
			}
		}
	}

	void vulkan_window::init_render_pass()
	{
		VkAttachmentDescription color_attachment{};
		color_attachment.format = m_window_data->m_swapchain_image_format;
		color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference color_attachment_ref{};
		color_attachment_ref.attachment = 0;
		color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment_ref;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo render_pass_info{};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		render_pass_info.attachmentCount = 1;
		render_pass_info.pAttachments = &color_attachment;
		render_pass_info.subpassCount = 1;
		render_pass_info.pSubpasses = &subpass;
		render_pass_info.dependencyCount = 1;
		render_pass_info.pDependencies = &dependency;

		if (vkCreateRenderPass(m_window_data->m_device, &render_pass_info, nullptr, &m_window_data->m_render_pass) != VK_SUCCESS)
		{
			SPARK_ERROR("[VULKAN] Failed to create render pass!");
			assert(false);
		}
	}

	void vulkan_window::init_imgui()
	{
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

			// Set up Dear ImGui style
			ImGui::StyleColorsDark();

			// Initialize ImGui for Vulkan
			ImGui_ImplGlfw_InitForVulkan(m_window, true);

			ImGui_ImplVulkan_InitInfo init_info = {};
			init_info.Instance = m_window_data->m_instance;
			init_info.PhysicalDevice = m_window_data->m_physical_device;
			init_info.Device = m_window_data->m_device;
			init_info.QueueFamily = find_queue_families(m_window_data->m_physical_device).m_graphics_family.value();
			init_info.Queue = m_window_data->m_graphics_queue;
			init_info.PipelineCache = VK_NULL_HANDLE;
			init_info.Allocator = nullptr;
			init_info.MinImageCount = 2;
			init_info.ImageCount = m_window_data->m_swapchain_images.size();
			ImGui_ImplVulkan_Init(&init_info);
	}

	void vulkan_window::init_framebuffers()
	{
		m_window_data->m_swapchain_framebuffers.resize(m_window_data->m_swapchain_image_views.size());


		for (uint64_t i = 0; i < m_window_data->m_swapchain_image_views.size(); i++)
		{
			VkImageView attachments[] = {
				m_window_data->m_swapchain_image_views[i]
			};

			VkFramebufferCreateInfo framebuffer_info{};
			framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebuffer_info.renderPass = m_window_data->m_render_pass;
			framebuffer_info.attachmentCount = 1;
			framebuffer_info.pAttachments = attachments;
			framebuffer_info.width = m_window_data->m_swapchain_extent.width;
			framebuffer_info.height = m_window_data->m_swapchain_extent.height;
			framebuffer_info.layers = 1;

			if (vkCreateFramebuffer(m_window_data->m_device, &framebuffer_info, nullptr, &m_window_data->m_swapchain_framebuffers[i]) != VK_SUCCESS)
			{
				SPARK_ERROR("[VULKAN] Failed to create framebuffer!");
				assert(false);
			}
		}
	}

	void vulkan_window::init_command_pool()
	{
		queue_family_indices indices = find_queue_families(m_window_data->m_physical_device);

		VkCommandPoolCreateInfo pool_info{};
		pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		pool_info.queueFamilyIndex = indices.m_graphics_family.value();

		if (vkCreateCommandPool(m_window_data->m_device, &pool_info, nullptr, &m_window_data->m_command_pool) != VK_SUCCESS)
		{
			SPARK_ERROR("[VULKAN] Failed to create command pool!");
			assert(false);
		}
	}

	void vulkan_window::record_command_buffer(VkCommandBuffer command_buffer, uint32_t image_index)
	{
		VkCommandBufferBeginInfo begin_info{};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = 0;
		begin_info.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS)
		{
			SPARK_ERROR("[VULKAN] Failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo render_pass_info{};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_info.renderPass = m_window_data->m_render_pass;
		render_pass_info.framebuffer = m_window_data->m_swapchain_framebuffers[image_index];
		render_pass_info.renderArea.offset = { 0, 0 };
		render_pass_info.renderArea.extent = m_window_data->m_swapchain_extent;

		VkClearValue clear_color = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		render_pass_info.clearValueCount = 1;
		render_pass_info.pClearValues = &clear_color;

		vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_window_data->m_graphics_pipeline);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_window_data->m_swapchain_extent.width);
		viewport.height = static_cast<float>(m_window_data->m_swapchain_extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(command_buffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = m_window_data->m_swapchain_extent;
		vkCmdSetScissor(command_buffer, 0, 1, &scissor);
		vkCmdDraw(command_buffer, 3, 1, 0, 0);
		vkCmdEndRenderPass(command_buffer);

		if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
		{
			SPARK_ERROR("[VULKAN] Failed to record command buffer!");
			assert(false);
		}
	}

	void vulkan_window::init_command_buffers()
	{
		m_window_data->m_command_buffers.resize(m_window_data->m_max_frames_in_flight);

		VkCommandBufferAllocateInfo alloc_info{};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.commandPool = m_window_data->m_command_pool;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandBufferCount = static_cast<uint32_t>(m_window_data->m_command_buffers.size());

		if (vkAllocateCommandBuffers(m_window_data->m_device, &alloc_info, &m_window_data->m_command_buffers[m_window_data->m_current_frame]) != VK_SUCCESS)
		{
			SPARK_ERROR("[VULKAN] Failed to allocate command buffers!");
		}
	}

	void vulkan_window::init_debug()
	{
		if (!m_enable_validation_layers)
			return;

		VkDebugUtilsMessengerCreateInfoEXT create_info;
		populate_debug_messenger(create_info);

		if (create_debug_utils_messenger_ext(m_window_data->m_instance, &create_info, nullptr, &m_window_data->m_debug_messenger) != VK_SUCCESS)
		{
			SPARK_ERROR("[VULKAN] Failed to set up debug messenger!");
			assert(false);
		}
	}

	void vulkan_window::event_callback(std::shared_ptr <event> _event)
	{
		publish_to_topic(WINDOW_EVENT_TOPIC, _event);
	}

	void vulkan_window::framebuffer_resize_callback(GLFWwindow* window, int32_t width, int32_t height)
	{
		auto window_data = reinterpret_cast<vulkan_window_data*>(glfwGetWindowUserPointer(window));
		window_data->m_framebuffer_resized = true;
	}

	void vulkan_window::close_event_callback(GLFWwindow* window)
	{
		opengl_window_data* _window_data = static_cast<opengl_window_data*>(glfwGetWindowUserPointer(window));
		auto event = std::make_shared<window_closed_event>();

		_window_data->m_event_callback(event);
	}

	void vulkan_window::key_event_callback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
	{
		opengl_window_data* _window_data = static_cast<opengl_window_data*>(glfwGetWindowUserPointer(window));

		switch (action)
		{
		case GLFW_PRESS:
		{
			auto event = std::make_shared<key_pressed_event>(key);
			_window_data->m_event_callback(event);
			break;
		}
		case GLFW_RELEASE:
		{
			auto event = std::make_shared<key_released_event>(key);
			_window_data->m_event_callback(event);
			break;
		}
		case GLFW_REPEAT:
		{
			auto event = std::make_shared<key_repeat_event>(key);
			_window_data->m_event_callback(event);
			break;
		}
		}
	}

	void vulkan_window::mouse_button_event_callback(GLFWwindow* window, int32_t button, int32_t action, int32_t mods)
	{
		opengl_window_data* _window_data = static_cast<opengl_window_data*>(glfwGetWindowUserPointer(window));

		switch (action)
		{
		case GLFW_PRESS:
		{
			auto event = std::make_shared<mouse_pressed_event>(button);
			_window_data->m_event_callback(event);
			break;
		}
		case GLFW_RELEASE:
		{
			auto event = std::make_shared<mouse_released_event>(button);
			_window_data->m_event_callback(event);
			break;
		}
		}
	}

	void vulkan_window::mouse_scroll_event_callback(GLFWwindow* window, float64_t xoffset, float64_t yoffset)
	{
		opengl_window_data* _window_data = static_cast<opengl_window_data*>(glfwGetWindowUserPointer(window));
		auto event = std::make_shared<mouse_scrolled_event>(xoffset, yoffset);

		_window_data->m_event_callback(event);
	}

	void vulkan_window::mouse_move_event_callback(GLFWwindow* window, float64_t xpos, float64_t ypos)
	{
		opengl_window_data* _window_data = static_cast<opengl_window_data*>(glfwGetWindowUserPointer(window));
		auto event = std::make_shared<mouse_moved_event>(xpos, ypos);

		_window_data->m_event_callback(event);
	}
}