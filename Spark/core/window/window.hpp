#ifndef SPARK_WINDOW_HPP
#define SPARK_WINDOW_HPP

#include <core/pch.hpp>
#include <core/event/event.hpp>

namespace Spark
{
	struct WindowData
	{
		std::string title = "Title";
		bool vsync = false;
		i32 width = 1080;
		i32 height = 1080;
		std::function<void(std::shared_ptr<IEvent>)> event_callback;
	};

	class IWindow
	{
	public:
		virtual ~IWindow() = default;
		virtual void CreateWindow(i32 width, i32 height, const std::string& title, std::function<void(std::shared_ptr<IEvent>)> cb) = 0;
		virtual void DestroyWindow() = 0;
		virtual void Update() = 0;
		virtual bool Running() = 0;
		virtual void SetTitle(const std::string&) = 0;
		virtual void SetSize(i32 width, i32 height) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;
		virtual WindowData&	GetWindowData() const = 0;
	};
}

#endif