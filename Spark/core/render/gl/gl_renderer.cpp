#include "gl_renderer.hpp"
#include <include/glad/glad.h>

namespace Spark
{
	void GLRenderer::Render()
	{
		glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}