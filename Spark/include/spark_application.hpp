#ifndef SPARK_APPLICATION_HPP
#define SPARK_APPLICATION_HPP

#include "spark_pch.hpp"
#include "spark_window.hpp"
#include "spark_event.hpp"
#include "spark_stopwatch.hpp"
#include "spark_graphics_api.hpp"
#include "spark_defer.hpp"
#include "spark_layer_stack.hpp"
#include "spark_renderer_layer.hpp"
#include "spark_command.hpp"

namespace spark
{
	template <typename ApiTy>
	concept ValidGraphicsApi = std::is_same_v<ApiTy, opengl::GL> || std::is_same_v<ApiTy, vulkan::VK> || std::is_same_v<ApiTy, directx::DX>;

	template <typename CTy>
	concept ValidCommand = std::is_base_of_v<ICommand, CTy>;

	template <ValidGraphicsApi ApiTy>
	class Application
	{
	public:
		Application(const std::string& title, const i32 win_width, const i32 win_height, bool win_vsync = false)
		{
			m_layer_stack.PushLayer<WindowLayer<ApiTy>>(title, win_width, win_height, win_vsync);
			m_layer_stack.PushLayer<RendererLayer<ApiTy>>();
		}

		Application& Start()
		{
			for (auto& layer : m_layer_stack)
			{
				layer->OnStart();
			}

			return *this;
		}

		Application& Run()
		{
			while (Running())
			{
				for (auto& layer : m_layer_stack)
				{
					layer->OnUpdate(m_dt);
				}
			}

			return *this;
		}

		template <ValidCommand T, typename... Args> 
		Application& SubmitCommand(const Args&... args)
		{
		}

		Application& DeltaTime(f32 dt)
		{
			m_dt = dt;

			return *this;
		}

		Application& Close()
		{
			for (const auto& layer : m_layer_stack)
			{
				layer->OnDetach();
			}

			return *this;
		}

		bool Running()
		{
			return dynamic_cast<WindowLayer<ApiTy>&>(m_layer_stack[0]).Running();
		}

	private:

		RendererLayer<ApiTy>& GetRenderer()
		{
			return dynamic_cast<RendererLayer<ApiTy>&>(m_layer_stack[1]);
		}

	private:
		LayerStack m_layer_stack;
		f32 m_dt = 0;
	};
}


#endif