#ifndef SPARK_GL_RENDERER_HPP
#define SPARK_GL_RENDERER_HPP

#include "spark_renderer.hpp"

namespace spark::opengl
{
    class SPARK_API GLRenderer : public IRenderer
    {
    public:
        GLRenderer() = default;
        ~GLRenderer() override = default;

        void Initialize() override;
        void BeginFrame() override;
        void EndFrame() override;
        void DrawSomething() override;
        void Shutdown() override;
        void RunRenderCommand(const RenderCommand& command) override;

        // Viewport management
        void SetViewport(uint32_t width, uint32_t height);

    private:
        uint32_t m_viewport_width = 800;  // Default width
        uint32_t m_viewport_height = 600; // Default height
    };
}

#endif // SPARK_GL_RENDERER_HPP
