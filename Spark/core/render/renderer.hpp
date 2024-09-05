#ifndef SPARK_RENDERER_HPP
#define SPARK_RENDERER_HPP

#include <core/api.hpp>
#include <core/ecs/entity.hpp>
#include <core/pch.hpp>
#include <core/scene/transform.hpp>
#include <core/window/framebuffer.hpp>

namespace Spark {
class Renderer {
  public:
	Renderer(GraphicsAPI gapi, Framebuffer& framebuffer)
		: m_gapi(gapi)
		, m_framebuffer(framebuffer) {}
	virtual ~Renderer()   = default;
	virtual void Render() = 0;

  protected:
	GraphicsAPI m_gapi;
	Framebuffer& m_framebuffer;
};

template <typename T>
concept IsRenderer = std::derived_from<T, Renderer>;
} // namespace Spark

#endif