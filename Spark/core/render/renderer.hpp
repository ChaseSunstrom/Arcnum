#ifndef SPARK_RENDERER_HPP
#define SPARK_RENDERER_HPP

#include <core/api.hpp>
#include <core/ecs/entity.hpp>
#include <core/pch.hpp>
#include <core/scene/transform.hpp>

namespace Spark {
class Renderer {
  public:
	Renderer(GraphicsAPI gapi)
		: m_gapi(gapi) {}
	~Renderer()           = default;
	virtual void Render() = 0;

  protected:
	GraphicsAPI m_gapi;
};

template <typename T>
concept IsRenderer = std::derived_from<T, Renderer>;
} // namespace Spark

#endif