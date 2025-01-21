// spark_renderer_layer.hpp
#ifndef SPARK_RENDERER_LAYER_HPP
#define SPARK_RENDERER_LAYER_HPP

#include "spark_layer.hpp"
#include "spark_renderer.hpp"
#include "spark_command_queue.hpp"

#include "spark_graphics_api.hpp"

#include "opengl/spark_gl_renderer.hpp"
// #include "vulkan/spark_vk_renderer.hpp"
// #include "directx/spark_dx_renderer.hpp"

namespace spark
{
	template<typename ApiTy>
	class RendererLayer : public ILayer
	{
	public:
		RendererLayer(CommandQueue<ApiTy>& command_queue)
			: m_command_queue(command_queue)
		{
			// Depending on ApiTy, create the correct renderer instance
			// This is pseudo-coded, you might use if constexpr or specializations
			if constexpr (IsGraphicsApi<ApiTy, opengl::GL>)
			{
				m_renderer = std::make_unique<opengl::GLRenderer>();
			}
			else if constexpr (IsGraphicsApi<ApiTy, directx::DX>)
			{
				//m_renderer = std::make_unique<directx::DXRenderer>();
			}
			else if constexpr (IsGraphicsApi<ApiTy, vulkan::VK>)
			{
				//m_renderer = std::make_unique<vulkan::VKRenderer>();
			}
		}

		~RendererLayer() override = default;

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

			m_command_queue.template FlushQueue<RenderCommand>([this](const RenderCommand& command)
				{
					this->m_renderer->RunRenderCommand(command);
				});

			m_renderer->EndFrame();
		}

		IRenderer& GetRenderer() { return *m_renderer; }

	private:
		// A pointer to the polymorphic renderer interface
		std::unique_ptr<IRenderer> m_renderer;
		CommandQueue<ApiTy>& m_command_queue;
	};
}

#endif // SPARK_RENDERER_LAYER_HPP
