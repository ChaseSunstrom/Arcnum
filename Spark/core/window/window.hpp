#ifndef SPARK_WINDOW_HPP
#define SPARK_WINDOW_HPP

#include <core/pch.hpp>
#include <core/event/event.hpp>
#include <core/event/event_handler.hpp>
#include "framebuffer.hpp"

namespace Spark {
	struct WindowData {
		WindowData(const String& title, bool vsync, i32 width, i32 height, EventHandler& event_handler)
			: title(title)
			, vsync(vsync)
			, width(width)
			, height(height)
			, event_handler(event_handler) {}

		String        title;
		bool          vsync;
		i32           width;
		i32           height;
		EventHandler& event_handler;
	};

	class Window {
	  public:
		Window(const String& title, i32 width, i32 height, EventHandler& event_handler, bool vsync = false)
			: m_window_data(MakeUnique<WindowData>(title, vsync, width, height, event_handler)) {}
		virtual ~Window()                                                                  = default;
		virtual void         CreateWindow(i32 width, i32 height, const String& title) = 0;
		virtual void         DestroyWindow()                                               = 0;
		virtual void         Update()                                                      = 0;
		virtual bool         Running()                                                     = 0;
		virtual void         SetTitle(const String&)                                  = 0;
		virtual void         SetSize(i32 width, i32 height)                                = 0;
		virtual void         SetVSync(bool enabled)                                        = 0;
		virtual bool         IsVSync() const                                               = 0;
		virtual WindowData&  GetWindowData() const                                         = 0;
		virtual Framebuffer& GetFrameBuffer() const                                        = 0;

	  protected:
		UniquePtr<WindowData> m_window_data;
	};

	template<typename _Ty>
	concept IsWindow = std::derived_from<_Ty, Window>;
} // namespace Spark

#endif