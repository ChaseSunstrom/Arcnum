#ifndef SPARK_GL_RENDERER_HPP
#define SPARK_GL_RENDERER_HPP

#include <core/api.hpp>
#include <core/pch.hpp>
#include <core/render/renderer.hpp>
#include <core/window/gl/gl_framebuffer.hpp>

namespace Spark {
class GLRenderer : public Renderer {
  public:
	GLRenderer(GraphicsAPI gapi, Framebuffer& framebuffer);
	~GLRenderer();
	void Render() override;
	void RenderFramebufferToScreen();
  private:
    u32 m_shader_program;
    u32 m_vao, m_vbo;
	u32 m_quad_vao, m_quad_vbo;
	u32 m_screen_shader_program;
	i32 m_window_width, m_window_height;
};
} // namespace Spark

#endif