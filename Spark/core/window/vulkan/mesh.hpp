#ifndef SPARK_MESH_HPP
#define SPARK_MESH_HPP

#include "../../ecs/component/component_types.hpp"

namespace spark
{
	struct vulkan_mesh : public mesh
	{
	public:
		vulkan_mesh(const std::vector<vertex>& vertices, const std::vector<u32>& indices);
		~vulkan_mesh();
		void create_mesh() override;
		void update(const std::vector<vertex>& vertices, const std::vector<u32>& indices);
	
		VkBuffer m_vertex_buffer;
		VkDeviceMemory m_vertex_buffer_memory;
	private:
		void create_vertex_buffer();
		u32 find_memory_type(u32 type_filter, VkMemoryPropertyFlags properties);
	};
}

#endif