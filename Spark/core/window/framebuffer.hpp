// framebuffer.hpp
#ifndef SPARK_FRAMEBUFFER_HPP
#define SPARK_FRAMEBUFFER_HPP
#include <core/pch.hpp>

namespace Spark {
class Framebuffer {
  public:
	Framebuffer(i32 width, i32 height)
		: m_width(width)
		, m_height(height) {}
	virtual ~Framebuffer()                     = default;
	virtual void Bind()                        = 0;
	virtual void Unbind()                      = 0;
	virtual u32 GetTexture() const             = 0;
	virtual void Resize(i32 width, i32 height) = 0;

	i32 GetWidth() const { return m_width; }
	i32 GetHeight() const { return m_height; }

  protected:
	i32 m_width;
	i32 m_height;
};
} // namespace Spark
#endif