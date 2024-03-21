#include "../app/app.hpp"

#include "instancer.hpp"

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

	void instancer::add_renderable(const std::string& mesh_name, const std::string& material_name, math::mat4& transform)
	{
		auto& material_map = m_renderables[mesh_name];
		if (!material_map[material_name])
		{
			material_map[material_name] = std::make_unique<transforms>();
		}

		material_map[material_name]->add_transform(transform);
	}

	void instancer::add_renderable(const std::string& mesh_name, const std::string& material_name, std::vector<math::mat4>& _transforms)
	{
		auto& material_map = m_renderables[mesh_name];
		if (!material_map[material_name])
		{
			material_map[material_name] = std::make_unique<transforms>();
		}

		for (auto& transform : _transforms)
		{
			material_map[material_name]->add_transform(transform);
		}
	}

	void instancer::bind_renderables(const std::string& mesh_name, const std::string& material_name)
	{
		window_data& win = application::get_window().get_window_data();

		glm::mat4 default_projection = glm::perspective(glm::radians(45.0f), (float)win.m_width / (float)win.m_height, 0.1f, 100.0f);
		glm::mat4 default_view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), // Camera is at (0,0,3), in World Space
											 glm::vec3(0.0f, 0.0f, 0.0f), // and looks at the origin
											 glm::vec3(0.0f, 1.0f, 0.0f)  // Head is up (set to 0,-1,0 to look upside-down)
		);


		auto& material_map = m_renderables[mesh_name];
		auto& trans_struct = material_map[material_name];

		if (!trans_struct) return; // Safeguard against null pointer

		material_manager& mat_man = application::get_material_manager();
		auto shader_program_id = mat_man.get_material(material_name).m_shader_program;

		glUseProgram(shader_program_id); // Ensure the shader program is active

		// Get uniform locations
		GLint loc_projection = glGetUniformLocation(shader_program_id, "projection");
		GLint loc_view = glGetUniformLocation(shader_program_id, "view");

		// Set the projection and view matrices
		glUniformMatrix4fv(loc_projection, 1, GL_FALSE, glm::value_ptr(default_projection));
		glUniformMatrix4fv(loc_view, 1, GL_FALSE, glm::value_ptr(default_view));

		material& mat = mat_man.get_material(material_name);
		set_uniform(mat, shader_program_id);

		mesh_manager& mesh_man = application::get_mesh_manager();
		auto vao_id = mesh_man.get_mesh(mesh_name).m_mesh.get_vao();
		glBindVertexArray(vao_id);

		trans_struct->update(); // This method binds and updates the VBO as needed
	}

	void instancer::render_instanced()
	{
		mesh_manager& mesh_man = application::get_mesh_manager();

		for (auto& mesh_item : m_renderables)
		{
			const auto& mesh_name = mesh_item.first;
			for (auto& material_item : mesh_item.second)
			{
				const auto& material_name = material_item.first;
				auto& transforms = material_item.second;

				if (transforms->m_data.empty()) continue; // Skip if no instances

				bind_renderables(mesh_name, material_name); // Prepare mesh and material for rendering

				auto index_count = static_cast<GLsizei>(mesh_man.get_mesh(mesh_name).m_mesh.m_indices.size());

				glDrawElementsInstanced(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0, transforms->m_data.size());
			}
		}
	}
}