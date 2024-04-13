#include "mesh.hpp"
#include "vulkan_window.hpp"

namespace spark
{
	vulkan_mesh::vulkan_mesh(const std::vector<vertex>& vertices, const std::vector<u32>& indices)
		: mesh(vertices, indices)
	{
		create_mesh();
	}

	vulkan_mesh::~vulkan_mesh()
	{
		auto& _vulkan_window = engine::get<vulkan_window>();

		vkDestroyBuffer(_vulkan_window.get_window_data().m_device, m_vertex_buffer, nullptr);
		vkFreeMemory(_vulkan_window.get_window_data().m_device, m_vertex_buffer_memory, nullptr);
	}

	void vulkan_mesh::update(const std::vector<vertex>& vertices, const std::vector<u32>& indices)
	{
		m_vertices = vertices;
		m_indices = indices;

		create_mesh();
	}

	void vulkan_mesh::create_mesh()
	{
		create_vertex_buffer();
	}

	void vulkan_mesh::create_vertex_buffer()
	{
		auto& _vulkan_window = engine::get<vulkan_window>();

		VkBufferCreateInfo buffer_info{};
		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_info.size = sizeof(m_vertices[0]) * m_vertices.size();
		buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(_vulkan_window.get_window_data().m_device, &buffer_info, nullptr, &m_vertex_buffer) != VK_SUCCESS)
		{
			SPARK_ERROR("[VULKAN] Failed to create mesh vertex buffer!");
			assert(false);
		}

		VkMemoryRequirements mem_requirements;
		vkGetBufferMemoryRequirements(_vulkan_window.get_window_data().m_device, m_vertex_buffer, &mem_requirements);

		VkMemoryAllocateInfo alloc_info{};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = mem_requirements.size;
		alloc_info.memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		if (vkAllocateMemory(_vulkan_window.get_window_data().m_device, &alloc_info, nullptr, &m_vertex_buffer_memory) != VK_SUCCESS)
		{
			SPARK_ERROR("[VULKAN] Failed to allocate mesh vertex buffer memory!");
			assert(false);
		}

		vkBindBufferMemory(_vulkan_window.get_window_data().m_device, m_vertex_buffer, m_vertex_buffer_memory, 0);

		void* data;
		vkMapMemory(_vulkan_window.get_window_data().m_device, m_vertex_buffer_memory, 0, buffer_info.size, 0, &data);
		std::memcpy(data, m_vertices.data(), (u64)buffer_info.size);
		vkUnmapMemory(_vulkan_window.get_window_data().m_device, m_vertex_buffer_memory);
	}

	u32 vulkan_mesh::find_memory_type(u32 type_filter, VkMemoryPropertyFlags properties)
	{
		auto& _vulkan_window = engine::get<vulkan_window>();

		VkPhysicalDeviceMemoryProperties mem_properties;
		vkGetPhysicalDeviceMemoryProperties(_vulkan_window.get_window_data().m_physical_device, &mem_properties);

		for (u32 i = 0; i < mem_properties.memoryTypeCount; i++)
		{
			if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		SPARK_ERROR("[VULKAN] Failed to find suitable memory type!");
		assert(false);
		return 0;
	}
}