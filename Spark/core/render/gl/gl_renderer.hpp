#ifndef SPARK_GL_RENDERER_HPP
#define SPARK_GL_RENDERER_HPP

#include <core/pch.hpp>
#include <core/render/renderer.hpp>
#include <core/api.hpp>

namespace Spark
{
	class GLRenderer : public Renderer
	{
	public:
		GLRenderer(GraphicsAPI gapi) : Renderer(gapi) {}
		void Render();
	private:

	};
}


#endif