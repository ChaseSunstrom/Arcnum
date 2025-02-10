#ifndef SPARK_GL_WINDOW_HPP
#define SPARK_GL_WINDOW_HPP

#include "spark_pch.hpp"
#include "spark_window.hpp"

typedef struct GLFWwindow GLFWwindow;

namespace spark::opengl
{
		class GLWindow : public Window
		{
		public:
			GLWindow(EventQueue& eq, const std::string& title, i32 width, i32 height, bool vsync = false);
			~GLWindow() override;
			void SetTitle(const std::string& title) override;
			void Update() override;
			void Close() override;
			bool IsOpen() const override;
			void SetVSync(bool enabled) override;
			bool IsVSync() const override;
			opaque GetNativeWindow() const override;
		private:
			GLFWwindow* m_window;
		};
}


#endif