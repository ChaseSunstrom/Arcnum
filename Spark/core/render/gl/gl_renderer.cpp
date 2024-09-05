#include "gl_renderer.hpp"
#include <core/render/gl/gl_material.hpp>
#include <core/render/gl/gl_mesh.hpp>
#include <core/util/gl.hpp>
#include <include/glm/gtc/matrix_transform.hpp>

namespace Spark {

	GLRenderer::GLRenderer(GraphicsAPI gapi, Framebuffer& framebuffer, Manager<Resource>& resource_manager)
		: Renderer(gapi, framebuffer, resource_manager)
		, m_window_width(framebuffer.GetWidth())
		, m_window_height(framebuffer.GetHeight()) {
		// Initialize G-Buffer
		auto& gl_fbuffer = dynamic_cast<GLFramebuffer&>(m_framebuffer);
		gl_fbuffer.AddColorAttachment(GL_RGB16F, GL_RGB, GL_FLOAT);      // Position
		gl_fbuffer.AddColorAttachment(GL_RGB16F, GL_RGB, GL_FLOAT);      // Normal
		gl_fbuffer.AddColorAttachment(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE); // Albedo
		gl_fbuffer.AddDepthAttachment();

		std::filesystem::path current_file(__FILE__);
		std::filesystem::path current_directory = current_file.parent_path();

		// Initialize shaders
		m_geometry_pass_shader = std::make_unique<GLRenderShader>(std::vector<std::pair<ShaderStage, const std::filesystem::path>>{
			{  ShaderStage::VERTEX, current_directory / "shaders/geometry_pass.vert"},
			{ShaderStage::FRAGMENT, current_directory / "shaders/geometry_pass.frag"}
        });

		m_geometry_pass_shader->Compile();

		m_lighting_pass_shader = std::make_unique<GLRenderShader>(std::vector<std::pair<ShaderStage, const std::filesystem::path>>{
			{  ShaderStage::VERTEX, current_directory / "shaders/lighting_pass.vert"},
			{ShaderStage::FRAGMENT, current_directory / "shaders/lighting_pass.frag"}
        });
		m_lighting_pass_shader->Compile();

		m_post_process_shader = std::make_unique<GLRenderShader>(std::vector<std::pair<ShaderStage, const std::filesystem::path>>{
			{  ShaderStage::VERTEX, current_directory / "shaders/post_process.vert"},
			{ShaderStage::FRAGMENT, current_directory / "shaders/post_process.frag"}
        });
		m_post_process_shader->Compile();

		m_screen_shader = std::make_unique<GLRenderShader>(std::vector<std::pair<ShaderStage, const std::filesystem::path>>{
			{  ShaderStage::VERTEX, current_directory / "shaders/screen.vert"},
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
		for (auto& [vao, instance_vbo] : m_instance_vbos) {
			glDeleteBuffers(1, &instance_vbo);
		}
	}

	void GLRenderer::Render(const Scene& scene) {
		RenderGeometryPass(scene);
		RenderLightingPass();
		RenderPostProcessPass();
		RenderFramebufferToScreen();
	}

	void GLRenderer::RenderGeometryPass(const Scene& scene) {
		auto& gl_fbuffer = dynamic_cast<GLFramebuffer&>(m_framebuffer);
		gl_fbuffer.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_geometry_pass_shader->Use();
		SetCommonUniforms(m_geometry_pass_shader.get());

		const auto& model_instances = scene.GetModelInstances();
		const auto& model_manager   = m_resource_manager.GetManager<StaticModel>();
		for (const auto& [model_name, transforms] : model_instances) {
			const StaticModel* model = &model_manager.Get(model_name);
			if (!model)
				continue;

			const GLStaticMesh& mesh     = static_cast<const GLStaticMesh&>(model->GetMesh());
			const GLMaterial*   material = static_cast<const GLMaterial*>(model->GetMaterial());

			RenderShader* shader         = material && material->HasCustomShader() ? material->GetCustomShader() : m_geometry_pass_shader.get();

			shader->Use();
			SetCommonUniforms(shader);

			if (material) {
				material->ApplyToShader(shader);
			}

			// Set up instanced rendering
			SetupInstancedRendering(mesh, transforms.size());

			// Update instance buffer with transforms
			UpdateInstanceBuffer(transforms);

			glBindVertexArray(mesh.GetVAO());
			glDrawElementsInstanced(GL_TRIANGLES, mesh.GetIndicesSize(), GL_UNSIGNED_INT, 0, transforms.size());
		}

		gl_fbuffer.Unbind();
	}

	void GLRenderer::RenderStaticModels(const std::vector<StaticModel*>& models) {
		for (const auto* model : models) {
			const GLStaticMesh& mesh     = static_cast<const GLStaticMesh&>(model->GetMesh());
			const GLMaterial*   material = static_cast<const GLMaterial*>(model->GetMaterial());

			RenderShader* shader         = material && material->HasCustomShader() ? material->GetCustomShader() : m_geometry_pass_shader.get();

			shader->Use();
			SetCommonUniforms(shader);

			// Set model matrix
			shader->SetMat4("u_model", model->GetModelMatrix());

			if (material) {
				material->ApplyToShader(shader);
			}

			glBindVertexArray(mesh.GetVAO());
			glDrawElements(GL_TRIANGLES, mesh.GetIndicesSize(), GL_UNSIGNED_INT, 0);
		}
	}

	void GLRenderer::RenderDynamicModels(const std::vector<DynamicModel*>& models) {
		for (const auto* model : models) {
			const GLDynamicMesh& mesh     = static_cast<const GLDynamicMesh&>(model->GetMesh());
			const GLMaterial*    material = static_cast<const GLMaterial*>(model->GetMaterial());

			RenderShader* shader          = material && material->HasCustomShader() ? material->GetCustomShader() : m_geometry_pass_shader.get();

			shader->Use();
			SetCommonUniforms(shader);

			// Set model matrix
			shader->SetMat4("u_model", model->GetModelMatrix());

			if (material) {
				material->ApplyToShader(shader);
			}

			glBindVertexArray(mesh.GetVAO());
			glDrawElements(GL_TRIANGLES, mesh.GetIndicesSize(), GL_UNSIGNED_INT, 0);
		}
	}

	void GLRenderer::RenderLightingPass() {
		auto& gl_fbuffer = dynamic_cast<GLFramebuffer&>(m_framebuffer);
		m_framebuffer.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_lighting_pass_shader->Use();

		// Bind G-Buffer textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gl_fbuffer.GetColorAttachment(0)); // Position
		m_lighting_pass_shader->SetI32("g_position", 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gl_fbuffer.GetColorAttachment(1)); // Normal
		m_lighting_pass_shader->SetI32("g_normal", 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gl_fbuffer.GetColorAttachment(2)); // Albedo
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
		glBindTexture(GL_TEXTURE_2D, dynamic_cast<GLFramebuffer&>(m_framebuffer).GetColorAttachment(0));
		m_screen_shader->SetI32("u_screen_texture", 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void GLRenderer::SetCommonUniforms(RenderShader* shader) {
		// Set view and projection matrices
		// TODO: Implement a camera system and set view/projection matrices here
		glm::mat4 view       = glm::mat4(1.0f);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (f32) m_window_width / (f32) m_window_height, 0.1f, 100.0f);

		shader->SetMat4("u_view", view);
		shader->SetMat4("u_projection", projection);
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

	void GLRenderer::SetupInstancedRendering(const GLStaticMesh& mesh, size_t instance_count) {
		u32 vao = mesh.GetVAO();
		if (m_instance_vbos.find(vao) == m_instance_vbos.end()) {
			u32 instance_vbo;
			glGenBuffers(1, &instance_vbo);
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, instance_vbo);

			// Set up attribute pointers for the instance matrix
			for (u32 i = 0; i < 4; ++i) {
				glEnableVertexAttribArray(3 + i); // Assuming attributes 0, 1, 2 are used for vertex data
				glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*) (sizeof(glm::vec4) * i));
				glVertexAttribDivisor(3 + i, 1);  // This attribute changes for each instance
			}

			m_instance_vbos[vao] = instance_vbo;
		}

		// No need to update the buffer here, as it will be done in UpdateInstanceBuffer
	}
} // namespace Spark