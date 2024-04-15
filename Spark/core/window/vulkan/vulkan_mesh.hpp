#ifndef SPARK_VULKAN_MESH_HPP
#define SPARK_VULKAN_MESH_HPP

#include "vulkan_window.hpp"
#include "../../ecs/component/mesh.hpp"
#include "../../ecs/component/vertex.hpp"

#

namespace spark
{
	struct vulkan_mesh :
			public mesh
	{
	public:
		struct uniform_buffer_object
		{
			std::any data;

			std::function<void()> create_func;

			std::function<void*()> data_func;

			std::function<u64()> size_func;

			VkBuffer buffer;

			VkDeviceMemory buffer_memory;

			template <typename UBOType>
			explicit uniform_buffer_object(const UBOType& ubo_data)
					:
					data(ubo_data), create_func([this, ubo_data]() { this->create_buffer<UBOType>(ubo_data); }), data_func(
					[this]()->void* { return &std::any_cast<UBOType&>(this->data); }), size_func([]()->u64 { return sizeof(UBOType); })
			{
				create_func(); 
			}

			template <typename UBOType>
			void create_buffer(const UBOType& ubo_data)
			{
				auto& vk_window = engine::get<vulkan_window>();
				VkDeviceSize buffer_size = sizeof(UBOType);

				create_vulkan_buffer(
						buffer_size,
						VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						this->buffer,
						this->buffer_memory);

				void* data_ptr;
				vkMapMemory(vk_window.get_window_data().m_device, this->buffer_memory, 0, buffer_size, 0, &data_ptr);
				std::memcpy(data_ptr, &ubo_data, buffer_size);
				vkUnmapMemory(vk_window.get_window_data().m_device, this->buffer_memory);
			}
		};

		template <typename... UBOTypes>
		vulkan_mesh(
				const std::vector <vertex>& vertices, const std::vector <u32>& indices, const UBOTypes& ... ubo_data)
				:
				mesh(vertices, indices)
		{
			create_mesh();
			(m_ubo_list.emplace_back(ubo_data), ...);
			update_descriptor_sets(ubo_data...);
		}

		~vulkan_mesh()
		{
			auto& vk_window = engine::get<vulkan_window>();

			for (auto& ubo: m_ubo_list)
			{
				vkDestroyBuffer(vk_window.get_window_data().m_device, ubo.buffer, nullptr);
				vkFreeMemory(vk_window.get_window_data().m_device, ubo.buffer_memory, nullptr);
			}

			vkDestroyBuffer(vk_window.get_window_data().m_device, m_index_buffer, nullptr);
			vkFreeMemory(vk_window.get_window_data().m_device, m_index_buffer_memory, nullptr);

			vkDestroyBuffer(vk_window.get_window_data().m_device, m_vertex_buffer, nullptr);
			vkFreeMemory(vk_window.get_window_data().m_device, m_vertex_buffer_memory, nullptr);
		}

		void create_mesh()
		{
			create_vertex_buffer();

			if (!m_indices.empty())
			{
				create_index_buffer();
			}
		}

		template <typename... UBOTypes>
		void update(
				const std::vector <vertex>& vertices, const std::vector <u32>& indices, const UBOTypes& ... ubo_data)
		{
			m_vertices = vertices;
			m_indices = indices;
			(m_ubo_list.emplace_back(ubo_data), ...);

			create_mesh();
			update_descriptor_sets();
		}

		void create_uniform_buffers()
		{
			for (auto& ubo: m_ubo_list)
			{
				ubo.create_func();
			}
		}

		template <typename UBOType>
		void add_ubo(const UBOType& ubo_data)
		{
			m_ubo_list.emplace_back(uniform_buffer_object(ubo_data));
		}

		template <typename UBOType>
		void* data_ubo_func(std::any& data)
		{
			return &std::any_cast<UBOType&>(data);
		}

		template <typename UBOType>
		u64 size_ubo_func(std::any& data)
		{
			return sizeof(UBOType);
		}

		VkBuffer m_vertex_buffer;

		VkDeviceMemory m_vertex_buffer_memory;

		VkBuffer m_index_buffer;

		VkDeviceMemory m_index_buffer_memory;

		std::vector <VkBuffer> m_uniform_buffers;

		std::vector <VkDeviceMemory> m_uniform_buffers_memory;

		std::vector<void*> m_uniform_buffers_mapped;

		std::vector <uniform_buffer_object> m_ubo_list;

	private:
		void create_vertex_buffer()
		{
			auto& vk_window = engine::get<vulkan_window>();

			VkBuffer staging_buffer;
			VkDeviceMemory staging_buffer_memory;
			VkDeviceSize buffer_size = sizeof(m_vertices[0]) * m_vertices.size();
			create_vulkan_buffer(
					buffer_size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					staging_buffer,
					staging_buffer_memory);

			void* data;
			vkMapMemory(vk_window.get_window_data().m_device, staging_buffer_memory, 0, buffer_size, 0, &data);
			std::memcpy(data, m_vertices.data(), (u64) buffer_size);
			vkUnmapMemory(vk_window.get_window_data().m_device, staging_buffer_memory);

			create_vulkan_buffer(
					buffer_size,
					VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					m_vertex_buffer,
					m_vertex_buffer_memory);
			copy_buffer(staging_buffer, m_vertex_buffer, buffer_size);

			vkDestroyBuffer(vk_window.get_window_data().m_device, staging_buffer, nullptr);
			vkFreeMemory(vk_window.get_window_data().m_device, staging_buffer_memory, nullptr);
		}

		void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
		{
			auto& vk_window = engine::get<vulkan_window>();

			VkCommandBufferAllocateInfo alloc_info { };
			alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			alloc_info.commandPool = vk_window.get_window_data().m_command_pool;
			alloc_info.commandBufferCount = 1;

			VkCommandBuffer command_buffer;
			vkAllocateCommandBuffers(vk_window.get_window_data().m_device, &alloc_info, &command_buffer);

			VkCommandBufferBeginInfo begin_info { };
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(command_buffer, &begin_info);

			VkBufferCopy copy_region { };
			copy_region.srcOffset = 0;
			copy_region.dstOffset = 0;
			copy_region.size = size;

			vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

			vkEndCommandBuffer(command_buffer);

			VkSubmitInfo submit_info { };
			submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submit_info.commandBufferCount = 1;
			submit_info.pCommandBuffers = &command_buffer;

			vkQueueSubmit(vk_window.get_window_data().m_graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
			vkQueueWaitIdle(vk_window.get_window_data().m_graphics_queue);

			vkFreeCommandBuffers(
					vk_window.get_window_data().m_device,
					vk_window.get_window_data().m_command_pool,
					1,
					&command_buffer);
		}

		void create_index_buffer()
		{
			auto& vk_window = engine::get<vulkan_window>();

			VkDeviceSize buffer_size = sizeof(m_indices[0]) * m_indices.size();

			VkBuffer staging_buffer;
			VkDeviceMemory staging_buffer_memory;
			create_vulkan_buffer(
					buffer_size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					staging_buffer,
					staging_buffer_memory);

			void* data;
			vkMapMemory(vk_window.get_window_data().m_device, staging_buffer_memory, 0, buffer_size, 0, &data);
			std::memcpy(data, m_indices.data(), (u64) buffer_size);
			vkUnmapMemory(vk_window.get_window_data().m_device, staging_buffer_memory);

			create_vulkan_buffer(
					buffer_size,
					VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					m_index_buffer,
					m_index_buffer_memory);

			copy_buffer(staging_buffer, m_index_buffer, buffer_size);

			vkDestroyBuffer(vk_window.get_window_data().m_device, staging_buffer, nullptr);
			vkFreeMemory(vk_window.get_window_data().m_device, staging_buffer_memory, nullptr);
		}

		static void create_vulkan_buffer(
				VkDeviceSize size,
				VkBufferUsageFlags usage,
				VkMemoryPropertyFlags properties,
				VkBuffer& buffer,
				VkDeviceMemory& buffer_memory)
		{
			auto& vk_window = engine::get<vulkan_window>();

			VkBufferCreateInfo buffer_info { };
			buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			buffer_info.size = size;
			buffer_info.usage = usage;
			buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if (vkCreateBuffer(vk_window.get_window_data().m_device, &buffer_info, nullptr, &buffer) != VK_SUCCESS)
			{
				SPARK_ERROR("[VULKAN] Failed to create buffer!");
				assert(false);
			}

			VkMemoryRequirements mem_requirements;
			vkGetBufferMemoryRequirements(vk_window.get_window_data().m_device, buffer, &mem_requirements);

			VkMemoryAllocateInfo alloc_info { };
			alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			alloc_info.allocationSize = mem_requirements.size;
			alloc_info.memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits, properties);

			if (vkAllocateMemory(vk_window.get_window_data().m_device, &alloc_info, nullptr, &buffer_memory) !=
			    VK_SUCCESS)
			{
				SPARK_ERROR("[VULKAN] Failed to allocate buffer memory!");
				assert(false);
			}

			vkBindBufferMemory(vk_window.get_window_data().m_device, buffer, buffer_memory, 0);
		}

		static u32 find_memory_type(u32 type_filter, VkMemoryPropertyFlags properties)
		{
			auto& vk_window = engine::get<vulkan_window>();

			VkPhysicalDeviceMemoryProperties mem_properties;
			vkGetPhysicalDeviceMemoryProperties(vk_window.get_window_data().m_physical_device, &mem_properties);

			for (u32 i = 0; i < mem_properties.memoryTypeCount; i++)
			{
				if ((type_filter & (1 << i)) &&
				    (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
				{
					return i;
				}
			}

			SPARK_ERROR("[VULKAN] Failed to find suitable memory type!");
			assert(false);
			return 0;
		}

		template <typename UBO>
		void create_uniform_buffers(const std::vector <UBO>& ubos)
		{
			auto& vk_window = engine::get<vulkan_window>();
			VkDeviceSize buffer_size = sizeof(UBO);

			std::vector <VkBuffer> buffers(ubos.size());
			std::vector <VkDeviceMemory> buffer_memories(ubos.size());

			for (size_t i = 0; i < ubos.size(); i++)
			{
				create_vulkan_buffer(
						buffer_size,
						VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						buffers[i],
						buffer_memories[i]);
				void* data;
				vkMapMemory(vk_window.get_window_data().m_device, buffer_memories[i], 0, buffer_size, 0, &data);
				memcpy(data, &ubos[i], buffer_size);
				vkUnmapMemory(vk_window.get_window_data().m_device, buffer_memories[i]);
			}
		}


		template <typename... UBOTypes, std::size_t... Is>
		void update_descriptor_set_impl(
				VkDevice device,
				VkDescriptorSet descriptor_set,
				std::index_sequence<Is...>,
				std::array<VkDescriptorBufferInfo, sizeof...(UBOTypes)>& buffer_infos,
				std::array<VkWriteDescriptorSet, sizeof...(UBOTypes)>& write_descriptor_sets,
				const UBOTypes& ... ubos)
		{
			// Lambda to create a VkDescriptorBufferInfo from a UBO
			auto createBufferInfo = [&](const auto& ubo, std::size_t index)
			{
				VkDescriptorBufferInfo bufferInfo { };
				bufferInfo.buffer = m_ubo_list[index].buffer;
				bufferInfo.offset = 0;
				bufferInfo.range = sizeof(ubo);
				return bufferInfo;
			};

			// Lambda to create a VkWriteDescriptorSet from a VkDescriptorBufferInfo
			auto createWriteDescriptorSet = [&](const VkDescriptorBufferInfo& bufferInfo, std::size_t index)
			{
				VkWriteDescriptorSet descriptorWrite { };
				descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrite.dstSet = descriptor_set;
				descriptorWrite.dstBinding = static_cast<u32>(index);
				descriptorWrite.dstArrayElement = 0;
				descriptorWrite.descriptorCount = 1;
				descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrite.pBufferInfo = &bufferInfo;
				return descriptorWrite;
			};

			// Unpack the ubos and populate the buffer info and write descriptor sets arrays
			(void(buffer_infos[Is] = createBufferInfo(ubos, Is)), ...);
			(void(write_descriptor_sets[Is] = createWriteDescriptorSet(buffer_infos[Is], Is)), ...);

			// Update the descriptor sets with the new buffer information
			vkUpdateDescriptorSets(device, sizeof...(UBOTypes), write_descriptor_sets.data(), 0, nullptr);
		}

		template <typename... UBOTypes>
		void update_descriptor_sets(const UBOTypes& ... ubo_data)
		{
			auto& vk_window = engine::get<vulkan_window>();
			VkDevice device = vk_window.get_window_data().m_device;

			for (u64 i = 0; i < vk_window.get_window_data().m_max_frames_in_flight; i++)
			{
				VkDescriptorSet descriptor_set = vk_window.get_window_data().m_descriptor_sets[i];

				// Create an array for buffer info and write descriptor for each UBO type.
				std::array<VkDescriptorBufferInfo, sizeof...(UBOTypes)> buffer_infos;
				std::array<VkWriteDescriptorSet, sizeof...(UBOTypes)> write_descriptor_sets;

				// Use index sequence to iterate over each UBO type and its corresponding index.
				update_descriptor_set_impl(device, descriptor_set, std::index_sequence_for <UBOTypes...>
				{ }, buffer_infos, write_descriptor_sets, ubo_data...);
			}
		}

	private:
		friend class uniform_buffer_object;
	};
}

#endif