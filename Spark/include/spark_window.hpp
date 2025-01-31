#ifndef SPARK_WINDOW_HPP
#define SPARK_WINDOW_HPP

#include "spark_pch.hpp"
#include "spark_layer.hpp"
#include "spark_defer.hpp"

#include "spark_graphics_api.hpp"

namespace spark
{
	struct WindowData
	{
		std::string title;
		i32 width;
		i32 height;
		bool vsync;
	};

	class SPARK_API Window
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

	class SPARK_API WindowLayer : public ILayer
	{
	public:
		WindowLayer(GraphicsApi gapi, const std::string& title, const i32 win_width, const i32 win_height, bool win_vsync)
		{
			DeferredState<WindowLayer&, GraphicsApi, std::string, i32, i32, bool>::Initialize(*this, gapi, title, win_width, win_height, win_vsync);
		}

		void OnStart() override
		{
			DeferredState<WindowLayer&, GraphicsApi, std::string, i32, i32, bool>::Execute(&WindowLayer::Initialize);
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

		void SetGraphicsApi(GraphicsApi gapi)
		{
			if (!m_window)
				return;

			i32 width = m_window->GetWidth();
			i32 height = m_window->GetHeight();
			std::string title = m_window->GetTitle();
			bool vsync = m_window->IsVSync();

			m_window.reset();

			Initialize(*this, gapi, title, width, height, vsync);
		}

		static void Initialize(WindowLayer& wl, GraphicsApi gapi, const std::string& title, const i32 win_width, const i32 win_height, bool win_vsync)
		{
			switch (gapi)
			{
			case GraphicsApi::OPENGL:
				opengl::GL::Initialize(wl, title, win_width, win_height, win_vsync);
				break;
			case GraphicsApi::DIRECTX:
				//directx::DX::Initialize(wl, title, win_width, win_height, win_vsync);
				break;
			case GraphicsApi::VULKAN:
				//vulkan::VK::Initialize(wl, title, win_width, win_height, win_vsync);
				break;
			}
		}

	private:		
		std::unique_ptr<Window> m_window;

		friend struct opengl::GL;
		friend struct vulkan::VK;
		friend struct directx::DX;
	};
}


#endif