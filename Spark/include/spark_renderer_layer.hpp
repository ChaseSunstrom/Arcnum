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
	class RendererLayer : public ILayer
	{
	public:
		RendererLayer(GraphicsApi gapi, CommandQueue& command_queue)
			: m_command_queue(command_queue)
		{
			
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
		CommandQueue& m_command_queue;
	};
}

#endif // SPARK_RENDERER_LAYER_HPP
