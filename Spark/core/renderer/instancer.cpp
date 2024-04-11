#include "../app/app.hpp"

#include "instancer.hpp"
#include "../user/camera.hpp"

namespace spark
{
	void transforms::add_transform(const transform& transform)
	{
		m_data.emplace_back(transform);
	}

	void transforms::update() 
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		glBufferData(GL_ARRAY_BUFFER, m_data.size() * sizeof(transform), m_data.data(), GL_DYNAMIC_DRAW);

		GLsizei vec4_size = sizeof(transform) / 4;
		for (GLuint i = 0; i < 4; ++i) 
		{
			glEnableVertexAttribArray(3 + i); 

			glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, 4 * vec4_size, reinterpret_cast<void*>(i * vec4_size));

			glVertexAttribDivisor(3 + i, 1);
		}
	}

	void transforms::update_render_transforms()
	{
		m_data.clear();
		for (auto& [entity, transform] : m_entity_transforms)
		{
			m_data.emplace_back(transform);
		}
	}

	void instancer::add_renderable(entity e, const scene& scene, const std::string& mesh_name, const std::string& material_name, const transform& _transform) 
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
		if (transform_storage && transform_storage->m_entity_transforms.erase(e) > 0) 
		{
			// If an entity was removed, refresh the rendering vector
			transform_storage->update_render_transforms();
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
			
			if (_ecs.has_component<transform_component>(e) &&
				_ecs.has_component<mesh_component>(e) &&
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
				m_renderables[mesh_name].find(material_name) != m_renderables[mesh_name].end()) {
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

#ifdef FIX_VULKAN
	void instancer::bind_renderables(const std::vector<std::unique_ptr<camera>>& cameras, const std::string& mesh_name, const std::string& material_name)
	{

		auto& material_map = m_renderables[mesh_name];
		auto& trans_struct = material_map[material_name];

		if (!trans_struct) return; // Safeguard against null pointer

		material_manager& mat_man = engine::get<material_manager>();
		auto shader_program_id = mat_man.get_material(material_name).m_shader_program;

		glUseProgram(shader_program_id); // Ensure the shader program is active

		// Get uniform locations
		GLint loc_projection = glGetUniformLocation(shader_program_id, "projection");
		GLint loc_view = glGetUniformLocation(shader_program_id, "view");

		for (auto& camera : cameras)
		{
			// Set the projection and view matrices
			glUniformMatrix4fv(loc_projection, 1, GL_FALSE, math::value_ptr(camera->get_projection_matrix()));
			glUniformMatrix4fv(loc_view, 1, GL_FALSE, math::value_ptr(camera->get_view_matrix()));

			material& mat = mat_man.get_material(material_name);
			set_uniform(mat, shader_program_id);

			mesh_manager& mesh_man = engine::get<mesh_manager>();
			auto vao_id = mesh_man.get_mesh(mesh_name).get_vao();
			glBindVertexArray(vao_id);
		}

		trans_struct->update(); // This method binds and updates the VBO as needed
	}
#endif
	void instancer::render_instanced(const std::vector<std::unique_ptr<camera>>& cameras, scene& scene)
	{
		for (auto& [mesh_name, material_map] : m_renderables)
		{
			for (auto& [material_name, transforms_ptr] : material_map)
			{
				// Bind the necessary resources for this mesh-material combination
				//bind_renderables(cameras, mesh_name, material_name);

				// Now update the instance transformation data
				transforms_ptr->update();

				// Perform the instanced draw call
				auto& mesh = engine::get<mesh_manager>().get_mesh(mesh_name);

				if (mesh.m_indices.size() == 0)
					glDrawArraysInstanced(GL_TRIANGLES, 0, mesh.m_vertices.size(), transforms_ptr->m_data.size());

				else
					glDrawElementsInstanced(GL_TRIANGLES, mesh.m_indices.size(), GL_UNSIGNED_INT, 0, transforms_ptr->m_data.size());
			}
		}
	}
}