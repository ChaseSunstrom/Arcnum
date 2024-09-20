#include "gl_renderer.hpp"
#include <core/render/gl/gl_material.hpp>
#include <core/render/gl/gl_mesh.hpp>
#include <core/util/gl.hpp>
#include <include/glm/gtc/matrix_transform.hpp>

namespace Spark {
	GLRenderer::GLRenderer(GraphicsAPI gapi, Framebuffer& framebuffer, Manager<Resource>& resource_manager)
		: Renderer(gapi, framebuffer, resource_manager)
		, m_g_framebuffer(dynamic_cast<GLFramebuffer&>(framebuffer))
		, m_window_width(framebuffer.GetWidth())
		, m_window_height(framebuffer.GetHeight()) {
		m_g_framebuffer.AddColorAttachment(GL_RGB16F, GL_RGB, GL_FLOAT);        // Position
		m_g_framebuffer.AddColorAttachment(GL_RGB16F, GL_RGB, GL_FLOAT);        // Normal
		m_g_framebuffer.AddColorAttachment(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE); // Albedo
		m_g_framebuffer.AddDepthAttachment();
		//m_g_framebuffer.Finalize();

		std::filesystem::path current_file(__FILE__);
		std::filesystem::path current_directory = current_file.parent_path();

		// Initialize shaders
		m_geometry_pass_shader = std::make_unique<GLRenderShader>(std::vector<std::pair<ShaderStage, const std::filesystem::path>>{
			{ShaderStage::VERTEX, current_directory / "shaders/geometry_pass.vert"},
			{ShaderStage::FRAGMENT, current_directory / "shaders/geometry_pass.frag"}
		});

		m_geometry_pass_shader->Compile();

		m_lighting_pass_shader = std::make_unique<GLRenderShader>(std::vector<std::pair<ShaderStage, const std::filesystem::path>>{
			{ShaderStage::VERTEX, current_directory / "shaders/lighting_pass.vert"},
			{ShaderStage::FRAGMENT, current_directory / "shaders/lighting_pass.frag"}
		});
		m_lighting_pass_shader->Compile();

		m_post_process_shader = std::make_unique<GLRenderShader>(std::vector<std::pair<ShaderStage, const std::filesystem::path>>{
			{ShaderStage::VERTEX, current_directory / "shaders/post_process.vert"},
			{ShaderStage::FRAGMENT, current_directory / "shaders/post_process.frag"}
		});
		m_post_process_shader->Compile();

		m_screen_shader = std::make_unique<GLRenderShader>(std::vector<std::pair<ShaderStage, const std::filesystem::path>>{
			{ShaderStage::VERTEX, current_directory / "shaders/screen.vert"},
			{ShaderStage::FRAGMENT, current_directory / "shaders/screen.frag"}
		});
		m_screen_shader->Compile();

		// Setup quad for fullscreen passes
		f32 quad_vertices[] = {-1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
		glGenVertexArrays(1, &m_quad_vao);
		glGenBuffers(1, &m_quad_vbo);
		glBindVertexArray(m_quad_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_quad_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*) 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*) (2 * sizeof(f32)));
	}

	GLRenderer::~GLRenderer() {
		glDeleteVertexArrays(1, &m_quad_vao);
		glDeleteBuffers(1, &m_quad_vbo);
		for (auto& [vao, instance_vbo] : m_instance_vbos) { glDeleteBuffers(1, &instance_vbo); }
	}

	void GLRenderer::Render(ConstPtr<Scene> scene) {
		RenderGeometryPass(scene);
		// RenderLightingPass();
		// RenderPostProcessPass();
		RenderFramebufferToScreen();
	}

	void GLRenderer::RenderGeometryPass(ConstPtr<Scene> scene) {
		m_g_framebuffer.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, m_window_width, m_window_height);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark gray background

		m_geometry_pass_shader->Use();
		SetCommonUniforms(m_geometry_pass_shader.get());

		const auto& model_instances = scene->GetModelInstances();
		const auto& model_manager   = m_resource_manager.GetManager<StaticModel>();
		for (const auto& [model_name, transforms] : model_instances) {
			ConstPtr<StaticModel> model = model_manager.Get(model_name);
			if (!model)
				continue;

			ConstPtr<GLStaticMesh> mesh     = ConstCast<GLStaticMesh>(model->GetMesh());
			ConstPtr<GLMaterial>   material = ConstCast<GLMaterial>(RefPtr(model->GetMaterial()));

			GLRenderShader* shader = material && material->HasCustomShader() ? static_cast<GLRenderShader*>(material->GetCustomShader()) : m_geometry_pass_shader.get();

			shader->Use();
			SetCommonUniforms(shader);

			if (material) { material->ApplyToShader(shader); }

			// Bind the VAO before setting up instanced rendering
			glBindVertexArray(mesh->GetVAO());

			// Set up instanced rendering
			SetupInstancedRendering(mesh, transforms.size());

			// Update instance buffer with transforms
			UpdateInstanceBuffer(transforms);

			if (mesh->GetIndicesSize() == 0) { glDrawArraysInstanced(GL_TRIANGLES, 0, mesh->GetVerticesSize(), transforms.size()); } else {
				glDrawElementsInstanced(GL_TRIANGLES, mesh->GetIndicesSize(), GL_UNSIGNED_INT, 0, transforms.size());
			}
		}

		m_g_framebuffer.Unbind();
	}

	void GLRenderer::RenderLightingPass() {
		m_framebuffer.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_lighting_pass_shader->Use();

		// Bind G-Buffer textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_g_framebuffer.GetColorAttachment(0)); // Position
		m_lighting_pass_shader->SetI32("g_position", 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_g_framebuffer.GetColorAttachment(1)); // Normal
		m_lighting_pass_shader->SetI32("g_normal", 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_g_framebuffer.GetColorAttachment(2)); // Albedo
		m_lighting_pass_shader->SetI32("g_albedo", 2);

		// Set lighting uniforms
		// TODO: Implement a light system and set light uniforms here

		glBindVertexArray(m_quad_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		m_framebuffer.Unbind();
	}

	void GLRenderer::RenderPostProcessPass() {
		// For now, we'll skip post-processing and render directly to the screen
		RenderFramebufferToScreen();
	}

	void GLRenderer::RenderFramebufferToScreen() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		m_screen_shader->Use();

		glBindVertexArray(m_quad_vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_g_framebuffer.GetColorAttachment(0));

		m_screen_shader->SetI32("u_screen_texture", 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void GLRenderer::SetCommonUniforms(GLRenderShader* shader) {
		RefPtr<Camera> camera = m_resource_manager.GetManager<Camera>().GetCurrentCamera();

		// Get uniform locations
		GLint view_loc = glGetUniformLocation(shader->GetId(), "u_view");
		GLint proj_loc = glGetUniformLocation(shader->GetId(), "u_projection");

		if (view_loc == -1 || proj_loc == -1) {
			LOG_ERROR("Unable to find uniform locations for view or projection matrices");
			return; // Early return to avoid setting invalid uniforms
		}

		// Get matrices from camera
		_MATH Mat4 view_matrix = camera->GetViewMatrix();
		_MATH Mat4 proj_matrix = camera->GetProjectionMatrix();

		// Set uniforms
		shader->SetMat4("u_view", view_matrix);
		shader->SetMat4("u_projection", proj_matrix);

		// Check for OpenGL errors
		GLenum error = glGetError();
		if (error != GL_NO_ERROR) { LOG_ERROR("OpenGL error occurred while setting uniforms: " << error); }
	}

	void GLRenderer::UpdateInstanceBuffer(const std::vector<glm::mat4>& transforms) {
		// Assuming we're using the last bound VAO's instance VBO
		u32 vao;
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (GLint*) &vao);

		auto it = m_instance_vbos.find(vao);
		if (it == m_instance_vbos.end()) {
			LOG_ERROR("No instance VBO found for VAO: " << vao);
			return;
		}

		u32 instance_vbo = it->second;
		glBindBuffer(GL_ARRAY_BUFFER, instance_vbo);
		glBufferData(GL_ARRAY_BUFFER, transforms.size() * sizeof(glm::mat4), transforms.data(), GL_DYNAMIC_DRAW);
	}

	void GLRenderer::SetupInstancedRendering(const GLStaticMesh& mesh, size_t  instance_count) {
		u32 vao = mesh.GetVAO();
		if (m_instance_vbos.find(vao) == m_instance_vbos.end()) {
			u32 instance_vbo;
			glGenBuffers(1, &instance_vbo);
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, instance_vbo);

			// Set up attribute pointers for the instance matrix
			for (u32 i = 0; i < 4; ++i) {
				glEnableVertexAttribArray(3 + i);
				glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*) (sizeof(glm::vec4) * i));
				glVertexAttribDivisor(3 + i, 1);
			}

			m_instance_vbos[vao] = instance_vbo;
		}
	}
} // namespace Spark