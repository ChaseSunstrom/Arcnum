#ifndef SPARK_GL_WINDOW_HPP
#define SPARK_GL_WINDOW_HPP

#include <core/pch.hpp>
#include <core/window/window.hpp>
#include <core/event/event.hpp>
#include <core/util/gl.hpp>

namespace Spark
{
	class GLWindow : public IWindow
	{
	public:
		GLWindow(i32 width, i32 height, const std::string& title, std::function<void(std::shared_ptr<IEvent>)> cb);
		~GLWindow();
		void DestroyWindow() override;
		void Update() override;
		bool Running() override;
		void SetTitle(const std::string&) override;
		void SetSize(i32 width, i32 height) override;
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;
		WindowData& GetWindowData() const override;
		GLFWwindow* GetNativeWindow() const;
	private:
		void CreateWindow(i32 width, i32 height, const std::string& title, std::function<void(std::shared_ptr<IEvent>)> cb);
	private:
		GLFWwindow* m_window;
		std::unique_ptr<WindowData> m_window_data;
	};
}

#endif