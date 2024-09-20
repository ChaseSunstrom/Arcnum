#include "gl_framebuffer.hpp"
#include <core/util/log.hpp>

namespace Spark {

	GLFramebuffer::GLFramebuffer(i32 width, i32 height)
		: Framebuffer(width, height)
		, m_fbo(0)
		, m_depth_attachment(0)
		, m_is_finalized(false) {
		glGenFramebuffers(1, &m_fbo);
	}

	GLFramebuffer::~GLFramebuffer() {
		glDeleteFramebuffers(1, &m_fbo);
		for (u32 texture : m_color_attachments) {
			glDeleteTextures(1, &texture);
		}
		if (m_depth_attachment != 0) {
			glDeleteTextures(1, &m_depth_attachment);
		}
	}

	void GLFramebuffer::Bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glViewport(0, 0, m_width, m_height);
	}

	void GLFramebuffer::Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

	void GLFramebuffer::Resize(i32 width, i32 height) {
		m_width  = width;
		m_height = height;

		for (size_t  i = 0; i < m_color_attachments.size(); ++i) {
			glBindTexture(GL_TEXTURE_2D, m_color_attachments[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		}

		if (m_depth_attachment != 0) {
			glBindTexture(GL_TEXTURE_2D, m_depth_attachment);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			LOG_ERROR("Framebuffer is not complete after resize!");
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GLFramebuffer::AddColorAttachment(GLenum internal_format, GLenum format, GLenum type) {
		if (m_is_finalized) {
			LOG_ERROR("Cannot add color attachment after framebuffer is finalized");
			return;
		}

		u32 texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, m_width, m_height, 0, format, type, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_color_attachments.size(), GL_TEXTURE_2D, texture, 0);

		m_color_attachments.push_back(texture);
	}

	void GLFramebuffer::AddDepthAttachment() {
		if (m_is_finalized) {
			LOG_ERROR("Cannot add depth attachment after framebuffer is finalized");
			return;
		}

		if (m_depth_attachment != 0) {
			LOG_ERROR("Depth attachment already exists");
			return;
		}

		glGenTextures(1, &m_depth_attachment);
		glBindTexture(GL_TEXTURE_2D, m_depth_attachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_attachment, 0);
	}

	void GLFramebuffer::Finalize() {
		if (m_is_finalized) {
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

		if (m_color_attachments.empty()) {
			LOG_ERROR("Framebuffer has no color attachments");
			return;
		}

		std::vector<GLenum> attachments;
		for (size_t  i = 0; i < m_color_attachments.size(); ++i) {
			attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
		}
		glDrawBuffers(attachments.size(), attachments.data());

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			LOG_ERROR("Framebuffer is not complete!");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		m_is_finalized = true;
	}

} // namespace Spark