#include "../app/app.hpp"

namespace spark
{
	void renderer::render(scene& scene)
	{
		clear_screen();

		math::vec4 background_color = scene.get_scene_config().m_background_color;

		glClearColor(background_color.x, background_color.y, background_color.z, background_color.w);
		check_gl_error("glClearColor");

		ecs& ecs = scene.get_ecs();

		component_array<mesh_component> mesh_array = ecs.get_component_array<mesh_component>();
		component_array<transform_component> transform_array = ecs.get_component_array<transform_component>();
		component_array<material_component> material_array = ecs.get_component_array<material_component>();

		mesh_manager& mesh_man = application::get_mesh_manager();
		material_manager& material_man = application::get_material_manager();

		//m_instancer->bind(transform_array.get_array());

		for (auto& mesh_i : mesh_array.get_array())
		{
			for (auto& material_i : material_array.get_array())
			{
				material& material = material_man.get_material(material_i.m_material_name);
				mesh& mesh = mesh_man.get_mesh(mesh_i.m_mesh_name).m_mesh;

				use_program(material.m_shader_program);
				bind_vertex_array(mesh.get_vao());

				//set_uniform(material, material.m_shader_program);

				// Assume model uniform location is known
				for (const auto& transform : transform_array.get_array()) {
					glm::mat4 model_matrix = transform.to_mat4();
					set_uniform("model", model_matrix, material.m_shader_program);
					glDrawElements(GL_TRIANGLES, mesh.m_indices.size(), GL_UNSIGNED_INT, nullptr);
				}
			}
		}
	}
}