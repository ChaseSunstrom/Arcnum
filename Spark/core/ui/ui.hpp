#ifndef SPARK_UI_HPP
#define SPARK_UI_HPP

#include <IMGUI/imgui.h>
#include <IMGUI/imgui_impl_glfw.h>
#include <IMGUI/imgui_impl_opengl3.h>

#include "../spark.hpp"

namespace spark
{
	class ui_component
	{
	public:
		virtual ~ui_component() = default;
		virtual void draw() = 0; // Each component must implement its own draw method
	};

	class button_component : public ui_component
	{
		std::string label;
		std::function<void()> on_click;

	public:
		button_component(const std::string& label, std::function<void()> on_click)
			: label(label), on_click(std::move(on_click))
		{}

		void draw() override
		{
			if (ImGui::Button(label.c_str()))
			{
				on_click();
			}
		}
	};

	class text_component : public ui_component
	{
		std::string text;

	public:
		text_component(const std::string& text) : text(text)
		{}

		void draw() override
		{
			ImGui::Text("%s", text.c_str());
		}
	};

	class slider_float_component : public ui_component
	{
		
	public:
		slider_float_component(const std::string& label, float* value, float min, float max)
			: m_label(label), m_value(value), m_min(min), m_max(max)
		{}

		void draw() override
		{
			ImGui::SliderFloat(m_label.c_str(), m_value, m_min, m_max);
		}
	private:
		std::string m_label;
		float32_t* m_value;
		float32_t m_min;
		float32_t m_max;
	};

	class window_component : public ui_component
	{
	public:
		window_component(const std::string& title, const ImVec2& position = ImVec2(), const ImVec2& size = ImVec2(), bool* p_open = nullptr)
			: title(title), position(position), size(size), p_open(p_open)
		{}

		void add_child(std::unique_ptr<ui_component>&& child)
		{
			children.push_back(std::move(child));
		}

		void draw() override
		{
			ImGui::SetNextWindowPos(position, ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);
			if (!ImGui::Begin(title.c_str(), p_open))
			{
				ImGui::End();
				return;
			}

			for (auto& child : children)
			{
				child->draw();
			}

			ImGui::End();
		}
	private:
		std::string title;
		std::vector<std::unique_ptr<ui_component>> children;
		ImVec2 position;
		ImVec2 size;
		bool* p_open = nullptr;
	};


	class checkbox_component : public ui_component
	{
		std::string label;
		bool* value;
	public:
		checkbox_component(const std::string& label, bool* value)
			: label(label), value(value)
		{}

		void draw() override
		{
			ImGui::Checkbox(label.c_str(), value);
		}
	};

	class slider_int_component : public ui_component
	{

	public:
		slider_int_component(const std::string& label, int32_t* value, int32_t min, int32_t max)
			: m_label(label), m_value(value), m_min(min), m_max(max)
		{}

		void draw() override
		{
			ImGui::SliderInt(m_label.c_str(), m_value, m_min, m_max);
		}
	private:
		std::string m_label;
		int32_t* m_value;
		int32_t m_min;
		int32_t m_max;
	};

	class ui_manager
	{
	public:
		static ui_manager& get()
		{
			static ui_manager instance;
			return instance;
		}

		template <typename... Args>
		void create_window(const std::string& name, Args&&... args)
		{
			auto window = std::make_unique<window_component>(std::forward<Args>(args)...);
			m_windows[name] = std::move(window);
		}

		// Modified to optionally add components to windows
		template <typename T, typename... Args>
		void create_component(const std::string& name = "", const std::string& label = "", Args&&... args)
		{
			auto component = std::make_unique<T>(label, std::forward<Args>(args)...);
			if (!name.empty() && m_windows.find(name) != m_windows.end())
			{
				m_windows[name]->add_child(std::move(component));
			}
			else
			{
				m_components.push_back(std::move(component));
			}
		}

		void on_start(GLFWwindow* window);
		void on_update();
		void on_end_update();
		void on_shutdown();
	private:
		ui_manager() = default;
		~ui_manager() = default;

		void draw_components()
		{
			for (auto& component : m_components)
			{
				component->draw();
			}
		}
	private:
		std::vector<std::unique_ptr<ui_component>> m_components;
		std::unordered_map<std::string, std::unique_ptr<window_component>> m_windows;
	};
}

#endif