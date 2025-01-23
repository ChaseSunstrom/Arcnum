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

    private:
        // Track GL-specific state if needed
        // e.g., function pointers, contexts, FBOs, etc.
    };
}

#endif // SPARK_GL_RENDERER_HPP
