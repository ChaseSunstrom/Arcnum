#ifndef SPARK_GL_WINDOW_HPP
#define SPARK_GL_WINDOW_HPP

#include <core/event/event.hpp>
#include <core/pch.hpp>
#include <core/util/gl.hpp>
#include <core/window/window.hpp>

namespace Spark
{
class GLWindow : public Window
{
  public:
	GLWindow(const std::string &title, i32 width, i32 height, EventHandler &event_handler, bool vsync = false);
	~GLWindow();
	void		DestroyWindow() override;
	void		Update() override;
	bool		Running() override;
	void		SetTitle(const std::string &) override;
	void		SetSize(i32 width, i32 height) override;
	void		SetVSync(bool enabled) override;
	bool		IsVSync() const override;
	WindowData &GetWindowData() const override;
	GLFWwindow *GetNativeWindow() const;

  private:
	void CreateWindow(i32 width, i32 height, const std::string &title);

  private:
	GLFWwindow *m_window;
};
} // namespace Spark

#endif