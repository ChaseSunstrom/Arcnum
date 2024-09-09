#ifndef SPARK_GL_FRAMEBUFFER_HPP
#define SPARK_GL_FRAMEBUFFER_HPP

#include <core/util/gl.hpp>
#include <core/window/framebuffer.hpp>

namespace Spark {

	class GLFramebuffer : public Framebuffer {
	  public:
		GLFramebuffer(i32 width, i32 height);
		~GLFramebuffer() override;

		void Bind() override;
		void Unbind() override;
		u32  GetTexture() const override { return m_color_attachments[0]; }
		void Resize(i32 width, i32 height) override;

		void AddColorAttachment(GLenum internal_format, GLenum format, GLenum type);
		void AddDepthAttachment();
		void Finalize();

		u32 GetColorAttachment(u32 index) const { return m_color_attachments[index]; }
		u32 GetDepthAttachment() const { return m_depth_attachment; }

	  private:
		u32              m_fbo;
		std::vector<u32> m_color_attachments;
		u32              m_depth_attachment;
		bool             m_is_finalized;
	};

} // namespace Spark

#endif // SPARK_GL_FRAMEBUFFER_HPP