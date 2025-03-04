#ifndef SPARK_RENDERER_LAYER_HPP
#define SPARK_RENDERER_LAYER_HPP

#include "spark_layer.hpp"
#include "spark_renderer.hpp"
#include "spark_command_queue.hpp"
#include "spark_delta_time.hpp"
#include "spark_render_command.hpp"
#include "spark_render_system.hpp"
#include "spark_graphics_api.hpp"

#include "opengl/spark_gl_renderer.hpp"
// #include "vulkan/spark_vk_renderer.hpp"
// #include "directx/spark_dx_renderer.hpp"

namespace spark
{
    class RendererLayer : public ILayer
    {
    public:
        RendererLayer(Coordinator& coordinator, GraphicsApi gapi, CommandQueue& command_queue)
            : m_command_queue(command_queue)
            , m_render_system(coordinator, command_queue)
            , m_viewport_width(800)  // Default width
            , m_viewport_height(600) // Default height
        {
            SetGraphicsApi(gapi);
        }

        ~RendererLayer() override = default;

        void OnAttach() override
        {
            m_renderer->Initialize();
            UpdateViewport(m_viewport_width, m_viewport_height);
        }

        void OnDetach() override
        {
            m_renderer->Shutdown();
        }

        void OnStart() override
        {
            // Initialize any render resources if needed
        }

        void OnUpdate(DeltaTime<f64> dt) override
        {
            if (!m_renderer) return;

            m_renderer->BeginFrame();

            // Let the render system handle all rendering

            // Process any manual render commands
            m_command_queue.template FlushQueue<RenderCommand>([this](const RenderCommand& command)
            {
                m_renderer->RunRenderCommand(command);
            });

            m_renderer->EndFrame();
        }

        void SetGraphicsApi(GraphicsApi gapi)
        {
            if (m_renderer)
            {
                m_renderer->Shutdown();
                m_renderer.reset();
            }

            switch (gapi)
            {
            case GraphicsApi::OPENGL:
                m_renderer = std::make_unique<opengl::GLRenderer>();
                break;
            case GraphicsApi::DIRECTX:
                //m_renderer = std::make_unique<directx::DXRenderer>();
                break;
            case GraphicsApi::VULKAN:
                //m_renderer = std::make_unique<vulkan::VKRenderer>();
                break;
            default:
                // Log error about invalid graphics API
                break;
            }

            if (m_renderer)
            {
                m_renderer->Initialize();
                UpdateViewport(m_viewport_width, m_viewport_height);
            }
        }

        void UpdateViewport(uint32_t width, uint32_t height)
        {
            if (!m_renderer) return;

            m_viewport_width = width;
            m_viewport_height = height;

            if (auto* gl_renderer = dynamic_cast<opengl::GLRenderer*>(m_renderer.get()))
            {
                gl_renderer->SetViewport(width, height);
            }
            // Add similar cases for other renderer types when implemented
        }

        IRenderer& GetRenderer() { return *m_renderer; }

        uint32_t GetViewportWidth() const { return m_viewport_width; }
        uint32_t GetViewportHeight() const { return m_viewport_height; }

    private:
        std::unique_ptr<IRenderer> m_renderer;
        CommandQueue& m_command_queue;
        RenderSystem m_render_system;
        uint32_t m_viewport_width;
        uint32_t m_viewport_height;
    };
}

#endif // SPARK_RENDERER_LAYER_HPP
