#include "../app/app.hpp"

#include "instancer.hpp"
#include "../window/window_manager.hpp"
#include "../window/vulkan/vulkan_window.hpp"
#include "../window/vulkan/vulkan_mesh.hpp"
#include "../user/camera.hpp"

namespace spark
{
	void transforms::add_transform(const transform& transform)
	{
		m_data.emplace_back(transform);
	}

	void transforms::update_render_transforms()
	{
		m_data.clear();
		for (auto& [entity, transform]: m_entity_transforms)
		{
			m_data.emplace_back(transform);
		}
	}

	void instancer::add_renderable(
			entity e,
			const scene& scene,
			const std::string& mesh_name,
			const std::string& material_name,
			const transform& _transform)
	{
		auto& material_map = m_renderables[mesh_name];
		if (!material_map[material_name])
		{
			material_map[material_name] = std::make_unique<transforms>();
		}
		material_map[material_name]->add_transform(_transform);
		m_entity_mesh_materials[e] = { mesh_name, material_name };

		//scene.get_octree().insert(const_cast<transform*>(&_transform));
	}

	void instancer::remove_renderable_for_entity(entity e)
	{
		auto [mesh_name, material_name] = m_entity_mesh_materials[e];

		auto& transform_storage = m_renderables[mesh_name][material_name];

		try
		{
			transform_storage->m_entity_transforms.erase(e);

			transform_storage->update_render_transforms();
		}
		catch (const std::exception& e)
		{
			SPARK_ERROR("[INSTANCER] Error removing renderable for entity: " << e.what());
		}
	}

	void instancer::on_notify(std::shared_ptr<event> event)
	{
		auto& _ecs = engine::get<ecs>();
		auto& _scene = engine::get<scene_manager>().get_current_scene();

		switch (event->m_type)
		{
			case ENTITY_CREATED_EVENT:
			{
				auto _entity_created_event = std::static_pointer_cast<entity_created_event>(event);
				entity e = _entity_created_event->m_entity;

				if (_ecs.has_component<transform_component>(e) && _ecs.has_component<mesh_component>(e) &&
				    _ecs.has_component<material_component>(e))
				{
					auto mesh_name = _ecs.get_component<mesh_component>(e).m_mesh_name;
					auto material_name = _ecs.get_component<material_component>(e).m_material_name;
					auto& transform = _ecs.get_component<transform_component>(e);

					add_renderable(e, _scene, mesh_name, material_name, transform);
				}
				break;
			}
			case ENTITY_UPDATED_EVENT:
			{
				auto _entity_updated_event = std::static_pointer_cast<entity_updated_event>(event);
				entity e = _entity_updated_event->m_entity;

				update_renderable(e, _scene);
				break;
			}
			case ENTITY_DESTROYED_EVENT:
			{
				auto _entity_destroyed_event = std::static_pointer_cast<entity_destroyed_event>(event);
				entity e = _entity_destroyed_event->m_entity;

				remove_renderable_for_entity(e);
				break;
			}
		}
	}

	void instancer::update_renderable(entity e, scene& scene)
	{
		// Check if the entity exists in the map
		if (m_entity_mesh_materials.find(e) != m_entity_mesh_materials.end())
		{
			// Get the mesh and material names associated with the entity
			const auto& [mesh_name, material_name] = m_entity_mesh_materials[e];

			// Check if the renderable object exists in the renderables map
			if (m_renderables.find(mesh_name) != m_renderables.end() &&
			    m_renderables[mesh_name].find(material_name) != m_renderables[mesh_name].end())
			{
				// Update the transformation for the renderable object
				auto& transform_storage = m_renderables[mesh_name][material_name];
				if (transform_storage)
				{
					// Get the transform component from the ECS
					auto& transform = engine::get<ecs>().get_component<transform_component>(e);

					// Update the transform in the storage
					transform_storage->m_entity_transforms[e] = transform;

					// Notify the transform storage to update its render transforms
					transform_storage->update_render_transforms();
				}
			}
		}
	}

	void instancer::render_instanced(const std::vector<std::unique_ptr<camera>>& cameras, scene& scene)
	{
		auto& vk_window = engine::get<vulkan_window>();

		for (auto& [mesh_name, material_map]: m_renderables)
		{
			for (auto& [material_name, transforms_ptr]: material_map)
			{
				// Perform the instanced draw call
				auto& mesh = engine::get<mesh_manager>().get_mesh(mesh_name);

				if (get_current_window_type() == window_type::VULKAN)
				{
					auto& _vulkan_mesh = dynamic_cast<vulkan_mesh&>(mesh);

					vkCmdBindPipeline(
							vk_window.get_window_data().m_command_buffers[vk_window.get_window_data().m_current_frame],
							VK_PIPELINE_BIND_POINT_GRAPHICS,
							vk_window.get_window_data().m_graphics_pipeline);

					VkBuffer vertex_buffers[] = { _vulkan_mesh.m_vertex_buffer };
					VkDeviceSize offsets[] = { 0 };
					vkCmdBindVertexBuffers(
							vk_window.get_window_data().m_command_buffers[vk_window.get_window_data().m_current_frame],
							0,
							1,
							vertex_buffers,
							offsets);

					

					if (!_vulkan_mesh.m_indices.empty())
					{
						vkCmdBindIndexBuffer(
								vk_window.get_window_data().m_command_buffers[vk_window.get_window_data()
								                                                       .m_current_frame],
								_vulkan_mesh.m_index_buffer,
								0,
								VK_INDEX_TYPE_UINT32);

						vkCmdBindDescriptorSets(
							vk_window.get_window_data().m_command_buffers[vk_window.get_window_data().m_current_frame],
							VK_PIPELINE_BIND_POINT_GRAPHICS,
							vk_window.get_window_data().m_pipeline_layout,
							0,
							1,
							&vk_window.get_window_data().m_descriptor_sets[vk_window.get_window_data().m_current_frame],
							0,
							nullptr);

						vkCmdDrawIndexed(
								vk_window.get_window_data().m_command_buffers[vk_window.get_window_data()
								                                                       .m_current_frame],
								_vulkan_mesh.m_indices.size(),
								transforms_ptr->m_data.size(),
								0,
								0,
								0);
					}

					else
					{
						vkCmdBindDescriptorSets(
							vk_window.get_window_data().m_command_buffers[vk_window.get_window_data().m_current_frame],
							VK_PIPELINE_BIND_POINT_GRAPHICS,
							vk_window.get_window_data().m_pipeline_layout,
							0,
							1,
							&vk_window.get_window_data().m_descriptor_sets[vk_window.get_window_data().m_current_frame],
							0,
							nullptr);

						vkCmdDraw(
								vk_window.get_window_data().m_command_buffers[vk_window.get_window_data()
								                                                       .m_current_frame],
								_vulkan_mesh.m_vertices.size(),
								transforms_ptr->m_data.size(),
								0,
								0);
					}
				}
			}
		}
	}
}