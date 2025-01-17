#ifndef SPARK_WINDOW_HPP
#define SPARK_WINDOW_HPP

#include "spark_pch.hpp"
#include "spark_layer.hpp"
#include "spark_defer.hpp"

#include "spark_gl.hpp"
#include "spark_vk.hpp"
#include "spark_dx.hpp"

namespace spark
{
	struct WindowData
	{
		std::string title;
		i32 width;
		i32 height;
		bool vsync;
	};

	class Window
	{
	public:
		Window(const std::string& title, i32 width, i32 height, bool vsync = false);
		virtual ~Window() = default;
		virtual void Update() = 0;
		virtual void Close() = 0;
		virtual bool IsOpen() const = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual void SetTitle(const std::string& title) = 0;
		virtual bool IsVSync() const = 0;
		virtual opaque GetNativeWindow() const = 0;
		i32 GetWidth() const;
		i32 GetHeight() const;
		const std::string& GetTitle() const;
	protected:
		WindowData m_data;
	};

	template <typename ApiTy>
	class WindowLayer : public Layer
	{
	public:
		WindowLayer(const std::string& title, const i32 win_width, const i32 win_height, bool win_vsync)
		{
			DeferredState<WindowLayer&, std::string, i32, i32, bool>::Initialize(*this, title, win_width, win_height, win_vsync);
		}

		void OnStart() override
		{
			DeferredState<WindowLayer&, std::string, i32, i32, bool>::Execute(&WindowLayer::Initialize);
		}

		void OnAttach() override {}
		void OnDetach() override {}

		void OnUpdate(f32 dt) override 
		{
			m_window->Update();
		}

		bool Running() const
		{
			return m_window->IsOpen();
		}

		static void Initialize(WindowLayer& wl, const std::string& title, const i32 win_width, const i32 win_height, bool win_vsync)
		{
			if constexpr (is_gl)
			{
				opengl::GL::Initialize(wl, title, win_width, win_height, win_vsync);
			}

			else if constexpr (is_dx)
			{
				directx::DX::Initialize(wl, title, win_width, win_height, win_vsync);
			}

			else if constexpr (is_vk)
			{

			}
		}

	private:		
		std::unique_ptr<Window> m_window;

		static constexpr bool is_gl = std::is_same_v<ApiTy, opengl::GL>;
		static constexpr bool is_vk = std::is_same_v<ApiTy, vulkan::VK>;
		static constexpr bool is_dx = std::is_same_v<ApiTy, directx::DX>;

		friend struct opengl::GL;
		friend struct vulkan::VK;
		friend struct directx::DX;
	};
}


#endif