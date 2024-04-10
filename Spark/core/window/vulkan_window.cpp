#include "vulkan_window.hpp"
#include "opengl_window.hpp"

namespace spark
{
	vulkan_window::vulkan_window() :
		window(window_type::VULKAN)
	{
		init_gl();
		init_vulkan();
		init_debug();
		create_surface();
		pick_physical_device();
		create_logical_device();
		create_swap_chain();
	}

	vulkan_window::~vulkan_window()
	{
		vkDestroySwapchainKHR(m_window_data->m_device, m_window_data->m_swapchain, nullptr);

		if (m_enable_validation_layers)
		{
			destroy_debug_utils_messenger_ext(m_window_data->m_instance, m_window_data->m_debug_messenger, nullptr);
		}

		vkDestroySurfaceKHR(m_window_data->m_instance, m_window_data->m_surface, nullptr);
		vkDestroyInstance(m_window_data->m_instance, nullptr);
		vkDestroyDevice(m_window_data->m_device, nullptr);
	}

	bool vulkan_window::check_validation_layer_support()
	{
		uint32_t layer_count;

		vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

		std::vector<VkLayerProperties> available_layers(layer_count);
		vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

		for (const char* layer_name : m_validation_layers)
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

	void vulkan_window::pre_draw()
	{}

	void vulkan_window::on_update()
	{}

	void vulkan_window::post_draw()
	{}

	bool vulkan_window::is_running()
	{
		return !glfwWindowShouldClose(m_window);
	}

	void vulkan_window::set_vsync(bool vsync)
	{}

	void vulkan_window::set_window_title(const std::string& title)
	{}

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

	void vulkan_window::pick_physical_device()
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

		std::set<std::string> required_extensions(m_device_extensions.begin(), m_device_extensions.end());

		for (const auto& extension : available_extensions)
		{
			required_extensions.erase(extension.extensionName);
		}

		return required_extensions.empty();
	}

	int32_t vulkan_window::rate_device(VkPhysicalDevice device)
	{
		VkPhysicalDeviceProperties device_properties;
		vkGetPhysicalDeviceProperties(device, &device_properties);

		VkPhysicalDeviceFeatures device_features;
		vkGetPhysicalDeviceFeatures(device, &device_features);

		int32_t score = 0;

		if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			score += 1000;

		score += device_properties.limits.maxImageDimension2D;

		if (!device_features.geometryShader)
			return 0;

		return score;
	}

	void vulkan_window::populate_debug_messenger(VkDebugUtilsMessengerCreateInfoEXT& create_info)
	{
		create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		create_info.pfnUserCallback = debug_callback;
	}

	void vulkan_window::init_gl()
	{
		glfwSetErrorCallback(glfw_error_callback);

		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_window = glfwCreateWindow(
			m_window_data->m_width, m_window_data->m_height, m_window_data->m_title.c_str(), NULL, NULL);

		glfwSetWindowUserPointer(m_window, m_window_data.get());

		glewInit();

		set_vsync(m_window_data->m_vsync);

		//glfw callbacks
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
			create_info.enabledLayerCount = static_cast<uint32_t>(m_validation_layers.size());
			create_info.ppEnabledLayerNames = m_validation_layers.data();

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

	void vulkan_window::create_swap_chain()
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
		if (result == VK_SUCCESS)
		{
			SPARK_INFO("[VULKAN] Swap chain created successfully.");
		}
		else
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


	void vulkan_window::create_logical_device()
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
		create_info.enabledExtensionCount = 0;

		if (m_enable_validation_layers)
		{
			create_info.enabledLayerCount = static_cast<uint32_t>(m_validation_layers.size());
			create_info.ppEnabledLayerNames = m_validation_layers.data();
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
		if (capabilities.currentExtent.width != UINT32_MAX)
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

	void vulkan_window::create_surface()
	{
		if (glfwCreateWindowSurface(m_window_data->m_instance, m_window, nullptr, &m_window_data->m_surface) != VK_SUCCESS)
		{
			SPARK_ERROR("[VULKAN] Failed to create window surface!");
			assert(false);
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
}