#include "spark_pch.hpp"
#include "opengl/spark_gl_renderer.hpp"
#include "spark_render_command.hpp"
#include <GL/glew.h> 

namespace spark::opengl
{
    void GLRenderer::Initialize()
    {
        // Initialize GLEW/Glad if not done earlier, etc.
        // glEnable(GL_DEPTH_TEST), etc.
    }

    void GLRenderer::BeginFrame()
    {
        // Clear buffers, set viewport, etc.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void GLRenderer::EndFrame()
    {
        // Possibly swap buffers if you manage that here
        // Usually the window layer might handle the actual glfwSwapBuffers()
    }

    void GLRenderer::RunRenderCommand(const RenderCommand& command)
    {
        const_cast<RenderCommand&>(command).Execute(*this);
    }

    void GLRenderer::DrawSomething()
    {
        // Example: use a shader, bind VBO, call glDrawElements, etc.
    }

    void GLRenderer::Shutdown()
    {
        // Cleanup OpenGL resources if needed
    }
}
