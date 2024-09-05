#ifndef SPARK_WINDOW_HPP
#define SPARK_WINDOW_HPP

#include "framebuffer.hpp"
#include <core/event/event.hpp>
#include <core/event/event_handler.hpp>
#include <core/pch.hpp>

namespace Spark {
struct WindowData {
	WindowData(const std::string& title, bool vsync, i32 width, i32 height, EventHandler& event_handler)
		: title(title)
		, vsync(vsync)
		, width(width)
		, height(height)
		, event_handler(event_handler) {}

	std::string title;
	bool vsync;
	i32 width;
	i32 height;
	EventHandler& event_handler;
};

class Window {
  public:
	Window(const std::string& title, i32 width, i32 height, EventHandler& event_handler, bool vsync = false)
		: m_window_data(std::make_unique<WindowData>(title, vsync, width, height, event_handler)) {}
	virtual ~Window()                                                          = default;
	virtual void CreateWindow(i32 width, i32 height, const std::string& title) = 0;
	virtual void DestroyWindow()                                               = 0;
	virtual void Update()                                                      = 0;
	virtual bool Running()                                                     = 0;
	virtual void SetTitle(const std::string&)                                  = 0;
	virtual void SetSize(i32 width, i32 height)                                = 0;
	virtual void SetVSync(bool enabled)                                        = 0;
	virtual bool IsVSync() const                                               = 0;
	virtual WindowData& GetWindowData() const                                  = 0;
	virtual Framebuffer& GetFrameBuffer() const                                = 0;

  protected:
	std::unique_ptr<WindowData> m_window_data;
};

template <typename T>
concept IsWindow = std::derived_from<T, Window>;
} // namespace Spark

#endif