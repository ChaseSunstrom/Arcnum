#include "gl_framebuffer.hpp"
#include <core/util/gl.hpp>

namespace Spark {
GLFramebuffer::GLFramebuffer(i32 width, i32 height)
	: Framebuffer(width, height)
	, m_fbo(0)
	, m_texture(0)
	, m_rbo(0) {
	// Generate and bind the framebuffer
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	// Create a color attachment texture
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

	// Create a renderbuffer object for depth and stencil attachment
	glGenRenderbuffers(1, &m_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

	// Check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		LOG_ERROR("Framebuffer is not complete!");
	}

	// Unbind the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLFramebuffer::~GLFramebuffer() {
	glDeleteFramebuffers(1, &m_fbo);
	glDeleteTextures(1, &m_texture);
	glDeleteRenderbuffers(1, &m_rbo);
}

void GLFramebuffer::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, m_width, m_height);
}

void GLFramebuffer::Unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLFramebuffer::Resize(i32 width, i32 height) {
	m_width  = width;
	m_height = height;

	// Resize texture
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	// Resize renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);

	// Check framebuffer completeness again
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		LOG_ERROR("Framebuffer is not complete after resize!");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace Spark