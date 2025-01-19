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
#include "spark_command_queue.hpp"
#include "spark_event_layer.hpp"

namespace spark
{
	template <typename ApiTy>
	constexpr bool ValidGraphicsApi = std::is_same_v<ApiTy, opengl::GL> || std::is_same_v<ApiTy, vulkan::VK> || std::is_same_v<ApiTy, directx::DX>;

	template <typename ApiTy>
	class Application
	{
		static_assert(ValidGraphicsApi<ApiTy>, "Application API must be a valid graphics API!");
	public:
		Application(const std::string& title, const i32 win_width, const i32 win_height, bool win_vsync = false)
		{
			m_layer_stack.PushLayer<WindowLayer<ApiTy>>(title, win_width, win_height, win_vsync);
			m_layer_stack.PushLayer<RendererLayer<ApiTy>>(m_command_queue);
			m_layer_stack.PushLayer<EventLayer>(m_event_queue);
		}

		Application& Start()
		{
			m_layer_stack.Start();
			return *this;
		}

		Application& Run()
		{
			while (Running())
			{
				m_layer_stack.Update(m_dt);
			}

			return *this;
		}

		template <ValidCommand T, typename... Args>
		Application& SubmitCommand(Args&&... args)
		{
			m_command_queue.template SubmitCommand<T>(std::forward<Args>(args)...);
			return *this;
		}

		template <ValidCommand T>
		Application& SubmitCommand(T&& command)
		{
			m_command_queue.template SubmitCommand<T>(std::forward<T>(command));
			return *this;
		}

        template <typename... Ts>
		Application& SubmitEvent(const Event<Ts...>& ev)
        {
			m_event_queue.SubmitEvent<Ts...>(ev);
			return *this;
        }

		template <typename T, typename... Ts>
		Application& SubmitEvent(const T& ev)
		{
			// Construct an Event<T, Ts...> from 'ev'
			// => your Event partial specialization must have a constructor
			//    that takes (const T&) or T (some data type).
			Event<T, Ts...> event(ev);

			// Then pass it to the queue as an Event<T, Ts...>
			m_event_queue.SubmitEvent<T, Ts...>(event);

			return *this;
		}


        template <typename... Ts>
		Application& Subscribe(std::function<void(Event<Ts...>&)> callback)
        {
			m_event_queue.Subscribe(callback);
			return *this;
        }

		Application& DispatchAll()
        {
			m_event_queue.DispatchAll();
			return *this;
        }

        template <typename... Ts>
		Application& ClearType()
        {
			m_event_queue.ClearType<Ts...>();
			return *this;
        }

		Application& DeltaTime(f32 dt)
		{
			m_dt = dt;
			return *this;
		}

		Application& Close()
		{
			m_layer_stack.Stop();
			return *this;
		}

		bool Running()
		{
			return GetWindowLayer().Running();
		}

		IRenderer& GetRenderer()
		{
			return GetRendererLayer().GetRenderer();
		}

	private:

		WindowLayer<ApiTy>& GetWindowLayer()
		{
			return *m_layer_stack.GetLayer<WindowLayer<ApiTy>>();
		}

		RendererLayer<ApiTy>& GetRendererLayer()
		{
			return *m_layer_stack.GetLayer<RendererLayer<ApiTy>>();
		}

		EventLayer& GetEventLayer()
		{
			return *m_layer_stack.GetLayer<EventLayer>();
		}

	private:
		LayerStack m_layer_stack;
		CommandQueue<ApiTy> m_command_queue;
		EventQueue m_event_queue;
		f32 m_dt = 0;
	};
}


#endif