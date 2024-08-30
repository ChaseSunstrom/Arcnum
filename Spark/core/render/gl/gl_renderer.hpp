#ifndef SPARK_GL_RENDERER_HPP
#define SPARK_GL_RENDERER_HPP

#include <core/api.hpp>
#include <core/pch.hpp>
#include <core/render/renderer.hpp>

namespace Spark
{
class GLRenderer : public Renderer
{
  public:
	GLRenderer(GraphicsAPI gapi) : Renderer(gapi) {}
	void Render() override;

  private:
};
} // namespace Spark

#endif