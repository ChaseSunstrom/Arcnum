#ifndef SPARK_GL_FRAMEBUFFER_HPP
#define SPARK_GL_FRAMEBUFFER_HPP
#include <core/window/framebuffer.hpp>
namespace Spark {
class GLFramebuffer : public Framebuffer {
  public:
	GLFramebuffer(i32 width, i32 height);
	~GLFramebuffer() override;
	void Bind() override;
	void Unbind() override;
	u32 GetTexture() const { return m_texture; }
	void Resize(i32 width, i32 height) override;

  private:
	u32 m_fbo;
	u32 m_texture;
	u32 m_rbo;
};
} // namespace Spark
#endif
