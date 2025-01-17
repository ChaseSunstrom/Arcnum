#ifndef SPARK_GL_RENDERER_HPP
#define SPARK_GL_RENDERER_HPP

#include "spark_renderer.hpp"

namespace spark::opengl
{
    class GLRenderer : public IRenderer
    {
    public:
        GLRenderer() = default;
        virtual ~GLRenderer() override = default;

        void Initialize() override;
        void BeginFrame() override;
        void EndFrame() override;
        void DrawSomething() override;
        void Shutdown() override;

    private:
        // Track GL-specific state if needed
        // e.g., function pointers, contexts, FBOs, etc.
    };
}

#endif // SPARK_GL_RENDERER_HPP
