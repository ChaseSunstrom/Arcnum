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
	}

	vulkan_window::~vulkan_window()
	{
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

		return indices.is_complete();
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

		std::vector<const char*> extensions = get_required_extensions();

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