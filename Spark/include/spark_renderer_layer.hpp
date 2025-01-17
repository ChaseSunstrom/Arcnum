// spark_renderer_layer.hpp
#ifndef SPARK_RENDERER_LAYER_HPP
#define SPARK_RENDERER_LAYER_HPP

#include "spark_layer.hpp"
#include "spark_renderer.hpp"

#include "opengl/spark_gl_renderer.hpp"
// #include "vulkan/spark_vk_renderer.hpp"
// #include "directx/spark_dx_renderer.hpp"

namespace spark
{
    template<typename ApiTy>
    class RendererLayer : public Layer
    {
    public:
        RendererLayer()
        {
            // Depending on ApiTy, create the correct renderer instance
            // This is pseudo-coded, you might use if constexpr or specializations
            if constexpr (std::is_same_v<ApiTy, opengl::GL>)
            {
                m_renderer = std::make_unique<opengl::GLRenderer>();
            }
            else if constexpr (std::is_same_v<ApiTy, vulkan::VK>)
            {
                //m_renderer = std::make_unique<vulkan::VKRenderer>();
            }
            else if constexpr (std::is_same_v<ApiTy, directx::DX>)
            {
                //m_renderer = std::make_unique<directx::DXRenderer>();
            }
        }

        virtual ~RendererLayer() override = default;

        void OnAttach() override
        {
            m_renderer->Initialize();
        }

        void OnDetach() override
        {
            m_renderer->Shutdown();
        }

        void OnStart() override
        {
            // Possibly do nothing here or set up extra state
        }

        void OnUpdate(float dt) override
        {
            m_renderer->BeginFrame();
            m_renderer->DrawSomething(); // or any custom drawing code
            m_renderer->EndFrame();
        }

    private:
        // A pointer to the polymorphic renderer interface
        std::unique_ptr<IRenderer> m_renderer;
    };
}

#endif // SPARK_RENDERER_LAYER_HPP
