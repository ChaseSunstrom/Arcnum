#include "../app/app.hpp"

#include "instancer.hpp"
#include "../user/camera.hpp"

namespace spark
{
	void transforms::add_transform(math::mat4& transform)
	{
		m_data.emplace_back(transform);
	}

	void transforms::update()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, m_data.size() * sizeof(math::mat4), m_data.data(), GL_DYNAMIC_DRAW);

		GLsizei vec4_size = sizeof(math::mat4) / 4;
		for (GLuint i = 0; i < 4; ++i)
		{
			glEnableVertexAttribArray(3 + i);
			glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, 4 * vec4_size, (void*)(i * vec4_size));
			glVertexAttribDivisor(3 + i, 1); // Instanced attribute
		}
	}

	void instancer::add_renderable(const scene& scene, const std::string& mesh_name, const std::string& material_name, transform_component& transform)
	{
		auto& material_map = m_renderables[mesh_name];
		if (!material_map[material_name])
		{
			material_map[material_name] = std::make_unique<transforms>();
		}

		material_map[material_name]->add_transform(transform.m_transform);
		scene.get_octree().insert(&transform);
	}

	void instancer::add_renderable(const scene& scene, const std::string& mesh_name, const std::string& material_name, std::vector<transform_component>& _transforms)
	{
		auto& material_map = m_renderables[mesh_name];
		if (!material_map[material_name])
		{
			material_map[material_name] = std::make_unique<transforms>();
		}

		for (auto& transform : _transforms)
		{
			material_map[material_name]->add_transform(transform.m_transform);
			scene.get_octree().insert(&transform);
		}
	}

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

	void instancer::on_notify(std::shared_ptr<event> event)
	{
		auto& _ecs = engine::get<ecs>();
		auto& _scene = engine::get<scene_manager>().get_current_scene();

		switch (event->m_type)
		{
		case ENTITY_CREATED_EVENT:
		{
			auto _entity_created_event = std::static_pointer_cast<entity_created_event>(event);
			auto& entity = _entity_created_event->m_entity;

			if (_ecs.has_component<transform_component>(entity) &&
				_ecs.has_component<mesh_component>(entity) &&
				_ecs.has_component<material_component>(entity))
			{
				std::string mesh_name = _entity_created_event->get_component<mesh_component>().m_mesh_name;
				std::string material_name = _entity_created_event->get_component<material_component>().m_material_name;
				transform_component transform = _entity_created_event->get_component<transform_component>();

				add_renderable(_scene, mesh_name, material_name, transform);
			}

			break;
		}
		case ENTITY_UPDATED_EVENT:
		{
			auto _entity_updated_event = std::static_pointer_cast<entity_updated_event>(event);
			auto& _entity = _entity_updated_event->m_entity;

			_ecs.remove_if([&_entity](const entity& ent)
				{
					return ent == _entity;
				});
		}
		case ENTITY_DESTROYED_EVENT:
		{

		}
		}
	}

	void instancer::remove_renderable_for_entity(entity e) {
		// Iterate through all mesh-material pairs

		auto& _ecs = engine::get<ecs>();

		auto& _transform_component =	_ecs.get_component<transform_component>(e);

		for (auto& mesh_pair : m_renderables) {
			for (auto& material_pair : mesh_pair.second) {
				// Iterate through all transforms associated with this mesh-material pair
				auto& transforms = material_pair.second->m_data;
				transforms.erase(
					std::remove_if(transforms.begin(), transforms.end(),
						[e, &_transform_component]()
						{
							// Assuming transform_component has a way to refer back to its entity
							return transform.entity_id == e;
						}),
					transforms.end());

				// If there are no more transforms left for this material, you might decide to remove the material entry altogether
				if (transforms.empty()) {
					material_pair.second.reset(); // Reset unique_ptr, effectively deleting it
				}
			}

			// Optionally, clean up mesh entries with no materials left
			auto& material_map = mesh_pair.second;
			for (auto it = material_map.begin(); it != material_map.end(); ) {
				if (!it->second) { // If the unique_ptr is reset
					it = material_map.erase(it);
				}
				else {
					++it;
				}
			}
		}
	}

	void instancer::render_instanced(const std::vector<std::unique_ptr<camera>>& cameras, scene& scene)
	{
		for (auto& camera : cameras)
		{
			octree& scene_octree = scene.get_octree();

			for (auto& mesh_item : m_renderables)
			{
				const auto& mesh_name = mesh_item.first;

				for (auto& material_item : mesh_item.second)
				{
					const auto& material_name = material_item.first;
					auto& trans_struct = material_item.second;

					std::vector<math::mat4> visible_transforms = m_renderables[mesh_name][material_name]->m_data;

					glBindBuffer(GL_ARRAY_BUFFER, trans_struct->m_vbo);
					glBufferData(GL_ARRAY_BUFFER, visible_transforms.size() * sizeof(math::mat4), visible_transforms.data(), GL_DYNAMIC_DRAW);

					bind_renderables(cameras, mesh_name, material_name); // Prepare mesh and material for rendering

					auto vertices = engine::get<mesh_manager>().get_mesh(mesh_name).m_vertices;

					glDrawArraysInstanced(GL_TRIANGLES, 0, vertices.size(), visible_transforms.size());
				}
			}
		}
	}

}