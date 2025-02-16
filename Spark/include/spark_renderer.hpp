#ifndef SPARK_RENDERER_HPP
#define SPARK_RENDERER_HPP

#include "spark_pch.hpp"
#include "spark_command.hpp"

namespace spark
{
    class RenderCommand;

    // A minimal interface for a renderer that can work with any API
    class SPARK_API IRenderer
    {
    public:
        virtual ~IRenderer() = default;

        // Initialize the renderer, e.g., create device, swapchain, etc.
        virtual void Initialize() = 0;

        // Called before each frame
        virtual void BeginFrame() = 0;

        // Called after all draw calls in a frame
        virtual void EndFrame() = 0;

        virtual void RunRenderCommand(const RenderCommand& command) = 0;

        // Example method to draw or submit something
        virtual void DrawSomething() = 0;

        // Cleanup any API-specific stuff
        virtual void Shutdown() = 0;
    };
}

#endif // SPARK_RENDERER_HPP
