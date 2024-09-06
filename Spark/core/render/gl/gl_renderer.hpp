#ifndef SPARK_GL_RENDERER_HPP
#define SPARK_GL_RENDERER_HPP

#include <core/api.hpp>
#include <core/pch.hpp>
#include <core/render/gl/gl_material.hpp>
#include <core/render/gl/gl_shader.hpp>
#include <core/render/model.hpp>
#include <core/render/renderer.hpp>
#include <core/resource/resource.hpp>
#include <core/scene/scene.hpp>
#include <core/window/gl/gl_framebuffer.hpp>
#include "gl_mesh.hpp"

namespace Spark {

	class GLRenderer : public Renderer {
	  public:
		GLRenderer(GraphicsAPI gapi, Framebuffer& framebuffer, Manager<Resource>& resource_manager);
		~GLRenderer();

		void Render(const Scene& scene) override;

	  private:
		void RenderGeometryPass(const Scene& scene);
		void RenderLightingPass();
		void RenderPostProcessPass();
		void RenderFramebufferToScreen();

		void SetupInstancedRendering(const GLStaticMesh& mesh, size_t instance_count);
		void UpdateInstanceBuffer(const std::vector<glm::mat4>& transforms);

		void SetCommonUniforms(GLRenderShader* shader);

	  private:
		GLFramebuffer&                  m_g_framebuffer;
		std::unique_ptr<GLRenderShader> m_geometry_pass_shader;
		std::unique_ptr<GLRenderShader> m_lighting_pass_shader;
		std::unique_ptr<GLRenderShader> m_post_process_shader;
		std::unique_ptr<GLRenderShader> m_screen_shader;
		u32                             m_quad_vao, m_quad_vbo;
		std::unordered_map<u32, u32>    m_instance_vbos; // VAO to instance VBO mapping
		i32                             m_window_width, m_window_height;
	};

} // namespace Spark

#endif // SPARK_GL_RENDERER_HPP