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
	public:
		button_component(const std::string& label, std::function<void()> on_click)
			: m_label(label), m_on_click(std::move(on_click))
		{}

		void draw() override
		{
			if (ImGui::Button(m_label.c_str()))
			{
				m_on_click();
			}
		}
	private:
		std::string m_label;
		std::function<void()> m_on_click;
	};

	class text_component : public ui_component
	{
	public:
		text_component(const std::string& text) : m_text(text)
		{}

		void draw() override
		{
			ImGui::Text("%s", m_text.c_str());
		}
	private:
		std::string m_text;
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

	class list_box_component : public ui_component
	{
	public:
		list_box_component(const std::string& label, int32_t* current_item, std::vector<std::string> items)
			: m_label(label), m_current_item(current_item), m_items(std::move(items))
		{}

		void draw() override
		{
			if (ImGui::BeginListBox(m_label.c_str()))
			{
				for (int32_t i = 0; i < m_items.size(); i++)
				{
					const bool is_selected = (*m_current_item == i);
					if (ImGui::Selectable(m_items[i].c_str(), is_selected))
						*m_current_item = i;

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}
		}

	private:
		std::string m_label;
		int32_t* m_current_item;
		std::vector<std::string> m_items;
	};

	class tree_component : public ui_component
	{
	public:
		tree_component(const std::string& label, std::function<void()> content)
			: m_label(label), m_content(std::move(content))
		{}

		void draw() override
		{
			if (ImGui::TreeNode(m_label.c_str()))
			{
				m_content();
				ImGui::TreePop();
			}
		}

	private:
		std::string m_label;
		std::function<void()> m_content; // Function to draw the content of the tree node
	};

	class color_picker_component : public ui_component
	{
	public:
		color_picker_component(const std::string& label, float32_t* color)
			: m_label(label), m_color(color)
		{}

		void draw() override
		{
			ImGui::ColorEdit3(m_label.c_str(), m_color);
		}

	private:
		std::string m_label;
		float32_t* m_color; // Should be an array of 3 floats (RGB)
	};

	class radio_button_component : public ui_component
	{
	public:
		radio_button_component(const std::string& label, int32_t* selected, std::vector<std::string> options)
			: m_label(label), m_selected(selected), m_options(std::move(options))
		{}

		void draw() override
		{
			ImGui::Text("%s", m_label.c_str());
			for (int32_t i = 0; i < m_options.size(); i++)
			{
				ImGui::RadioButton(m_options[i].c_str(), m_selected, i);
			}
		}

	private:
		std::string m_label;
		int32_t* m_selected;
		std::vector<std::string> m_options;
	};

	class window_component : public ui_component
	{
	public:
		window_component(const std::string& title, const ImVec2& position = ImVec2(), const ImVec2& size = ImVec2(), bool* p_open = nullptr)
			: m_title(title), m_position(position), m_size(size), m_p_open(p_open)
		{}

		void add_child(std::unique_ptr<ui_component>&& child)
		{
			m_children.push_back(std::move(child));
		}

		void draw() override
		{
			ImGui::SetNextWindowPos(m_position, ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(m_size, ImGuiCond_FirstUseEver);
			if (!ImGui::Begin(m_title.c_str(), m_p_open))
			{
				ImGui::End();
				return;
			}

			for (auto& child : m_children)
			{
				child->draw();
			}

			ImGui::End();
		}
	private:
		std::string m_title;
		std::vector<std::unique_ptr<ui_component>> m_children;
		ImVec2 m_position;
		ImVec2 m_size;
		bool* m_p_open = nullptr;
	};

	class toggle_switch_component : public ui_component
	{
	public:
		toggle_switch_component(const std::string& label, bool* value)
			: m_label(label), m_value(value)
		{}

		void draw() override
		{
			ImGui::Text("%s", m_label.c_str());
			ImGui::SameLine();
			if (ImGui::Button(*m_value ? "ON" : "OFF"))
			{
				*m_value = !*m_value;
			}
		}

	private:
		std::string m_label;
		bool* m_value;
	};

	class dropdown_component : public ui_component
	{
	public:
		dropdown_component(const std::string& label, int32_t* current_item, std::vector<std::string> items)
			: m_label(label), m_current_item(current_item), m_items(std::move(items))
		{
			for (auto& item : m_items)
			{
				m_items_cstr.push_back(item.c_str());
			}
		}

		void draw() override
		{
			if (ImGui::BeginCombo(m_label.c_str(), m_items_cstr[*m_current_item]))
			{
				for (int32_t i = 0; i < m_items_cstr.size(); i++)
				{
					bool is_selected = (*m_current_item == i);
					if (ImGui::Selectable(m_items_cstr[i], is_selected))
					{
						*m_current_item = i;
					}

					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}

	private:
		std::string m_label;
		int32_t* m_current_item;
		std::vector<std::string> m_items;
		std::vector<const char*> m_items_cstr;
	};

	class progress_bar_component : public ui_component
	{
	public:
		progress_bar_component(const std::string& label, float32_t value, const ImVec2& size = ImVec2(-1, 0))
			: m_label(label), m_value(value), m_size(size)
		{}

		void draw() override
		{
			ImGui::Text("%s", m_label.c_str());
			ImGui::ProgressBar(m_value, m_size);
		}

	private:
		std::string m_label;
		float32_t m_value;
		ImVec2 m_size;
	};

	class text_input_component : public ui_component
	{
	public:
		text_input_component(const std::string& label, char* buffer, size_t buffer_size)
			: m_label(label), m_buffer(buffer), m_buffer_size(buffer_size)
		{}

		void draw() override
		{
			ImGui::InputText(m_label.c_str(), m_buffer, m_buffer_size);
		}

	private:
		std::string m_label;
		char* m_buffer;
		size_t m_buffer_size;
	};

	class checkbox_component : public ui_component
	{
	public:
		checkbox_component(const std::string& label, bool* value)
			: m_label(label), m_value(value)
		{}

		void draw() override
		{
			ImGui::Checkbox(m_label.c_str(), m_value);
		}
	private:
		std::string m_label;
		bool* m_value;
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

	struct ui_theme
	{
		struct color_scheme
		{
			ImVec4 m_text;
			ImVec4 m_text_disabled;
			ImVec4 m_window_bg;
			ImVec4 m_child_bg;
			ImVec4 m_popup_bg;
			ImVec4 m_border;
			ImVec4 m_border_shadow;
			ImVec4 m_frame_bg;
			ImVec4 m_frame_bg_hovered;
			ImVec4 m_frame_bg_active;
			ImVec4 m_title_bg;
			ImVec4 m_title_bg_active;
			ImVec4 m_title_bg_collapsed;
			ImVec4 m_menu_bar_bg;
			ImVec4 m_scrollbar_bg;
			ImVec4 m_scrollbar_grab;
			ImVec4 m_scrollbar_grab_hovered;
			ImVec4 m_scrollbar_grab_active;
			ImVec4 m_check_mark;
			ImVec4 m_slider_grab;
			ImVec4 m_slider_grab_active;
			ImVec4 m_button;
			ImVec4 m_button_hovered;
			ImVec4 m_button_active;
			ImVec4 m_header;
			ImVec4 m_header_hovered;
			ImVec4 m_header_active;
			ImVec4 m_separator;
			ImVec4 m_separator_hovered;
			ImVec4 m_separator_active;
			ImVec4 m_resize_grip;
			ImVec4 m_resize_grip_hovered;
			ImVec4 m_resize_grip_active;
			ImVec4 m_tab;
			ImVec4 m_tab_hovered;
			ImVec4 m_tab_active;
			ImVec4 m_tab_unfocused;
			ImVec4 m_tab_unfocused_active;
			ImVec4 m_plot_lines;
			ImVec4 m_plot_lines_hovered;
			ImVec4 m_plot_histogram;
			ImVec4 m_plot_histogram_hovered;
			ImVec4 m_text_selected_bg;
			ImVec4 m_drag_drop_target;
			ImVec4 m_nav_highlight;
			ImVec4 m_nav_windowing_highlight;
			ImVec4 m_nav_windowing_dim_bg;
			ImVec4 m_modal_window_dim_bg;

			// Initialize with default values
			color_scheme()
				: m_text(ImGui::GetStyle().Colors[ImGuiCol_Text]),
				m_text_disabled(ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]),
				// Continue initialization for all colors...
				m_modal_window_dim_bg(ImGui::GetStyle().Colors[ImGuiCol_ModalWindowDimBg])
			{}
		} m_colors;

		// Style variables
		float32_t m_window_rounding;
		float32_t m_frame_rounding;
		ImVec2 m_window_padding;
		ImVec2 m_frame_padding;
		ImVec2 m_item_spacing;
		ImVec2 m_item_inner_spacing;
		float32_t m_scrollbar_size;
		float32_t m_scrollbar_rounding;
		float32_t m_scale;
		float32_t m_grab_rounding;
		float32_t m_curve_tessellation_tol;
		bool m_anti_aliased_lines;
		bool m_anti_aliased_fill;


		// Default constructor to initialize style variables with ImGui defaults
		ui_theme()
			: m_window_rounding(ImGui::GetStyle().WindowRounding),
			m_frame_rounding(ImGui::GetStyle().FrameRounding),
			m_window_padding(ImGui::GetStyle().WindowPadding),
			m_frame_padding(ImGui::GetStyle().FramePadding),
			m_item_spacing(ImGui::GetStyle().ItemSpacing),
			m_item_inner_spacing(ImGui::GetStyle().ItemInnerSpacing),
			m_scrollbar_size(ImGui::GetStyle().ScrollbarSize),
			m_scrollbar_rounding(ImGui::GetStyle().ScrollbarRounding)
		{}

		// Apply the theme to ImGui
		void apply() const
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.WindowRounding = m_window_rounding;
			style.FrameRounding = m_frame_rounding;
			style.WindowPadding = m_window_padding;
			style.FramePadding = m_frame_padding;
			style.ItemSpacing = m_item_spacing;
			style.ItemInnerSpacing = m_item_inner_spacing;
			style.ScrollbarSize = m_scrollbar_size;
			style.ScrollbarRounding = m_scrollbar_rounding;

			// Apply colors
			style.Colors[ImGuiCol_Text] = m_colors.m_text;
			style.Colors[ImGuiCol_TextDisabled] = m_colors.m_text_disabled;
			style.GrabRounding = m_grab_rounding;
			style.AntiAliasedLines = m_anti_aliased_lines;
			style.AntiAliasedFill = m_anti_aliased_fill;
			style.CurveTessellationTol = m_curve_tessellation_tol;

			// Fonts and scaling
			style.ScaleAllSizes(m_scale);

			// Apply colors (continuing from previously set colors)
			style.Colors[ImGuiCol_WindowBg] = m_colors.m_window_bg;
			style.Colors[ImGuiCol_ChildBg] = m_colors.m_child_bg;
			style.Colors[ImGuiCol_PopupBg] = m_colors.m_popup_bg;
			style.Colors[ImGuiCol_Border] = m_colors.m_border;
			style.Colors[ImGuiCol_BorderShadow] = m_colors.m_border_shadow;
			style.Colors[ImGuiCol_FrameBg] = m_colors.m_frame_bg;
			style.Colors[ImGuiCol_FrameBgHovered] = m_colors.m_frame_bg_hovered;
			style.Colors[ImGuiCol_FrameBgActive] = m_colors.m_frame_bg_active;
			style.Colors[ImGuiCol_TitleBg] = m_colors.m_title_bg;
			style.Colors[ImGuiCol_TitleBgActive] = m_colors.m_title_bg_active;
			style.Colors[ImGuiCol_TitleBgCollapsed] = m_colors.m_title_bg_collapsed;
			style.Colors[ImGuiCol_MenuBarBg] = m_colors.m_menu_bar_bg;
			style.Colors[ImGuiCol_ScrollbarBg] = m_colors.m_scrollbar_bg;
			style.Colors[ImGuiCol_ScrollbarGrab] = m_colors.m_scrollbar_grab;
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = m_colors.m_scrollbar_grab_hovered;
			style.Colors[ImGuiCol_ScrollbarGrabActive] = m_colors.m_scrollbar_grab_active;
			style.Colors[ImGuiCol_CheckMark] = m_colors.m_check_mark;
			style.Colors[ImGuiCol_SliderGrab] = m_colors.m_slider_grab;
			style.Colors[ImGuiCol_SliderGrabActive] = m_colors.m_slider_grab_active;
			style.Colors[ImGuiCol_Button] = m_colors.m_button;
			style.Colors[ImGuiCol_ButtonHovered] = m_colors.m_button_hovered;
			style.Colors[ImGuiCol_ButtonActive] = m_colors.m_button_active;
			style.Colors[ImGuiCol_Header] = m_colors.m_header;
			style.Colors[ImGuiCol_HeaderHovered] = m_colors.m_header_hovered;
			style.Colors[ImGuiCol_HeaderActive] = m_colors.m_header_active;
			style.Colors[ImGuiCol_Separator] = m_colors.m_separator;
			style.Colors[ImGuiCol_SeparatorHovered] = m_colors.m_separator_hovered;
			style.Colors[ImGuiCol_SeparatorActive] = m_colors.m_separator_active;
			style.Colors[ImGuiCol_ResizeGrip] = m_colors.m_resize_grip;
			style.Colors[ImGuiCol_ResizeGripHovered] = m_colors.m_resize_grip_hovered;
			style.Colors[ImGuiCol_ResizeGripActive] = m_colors.m_resize_grip_active;
			style.Colors[ImGuiCol_Tab] = m_colors.m_tab;
			style.Colors[ImGuiCol_TabHovered] = m_colors.m_tab_hovered;
			style.Colors[ImGuiCol_TabActive] = m_colors.m_tab_active;
			style.Colors[ImGuiCol_TabUnfocused] = m_colors.m_tab_unfocused;
			style.Colors[ImGuiCol_TabUnfocusedActive] = m_colors.m_tab_unfocused_active;
			style.Colors[ImGuiCol_PlotLines] = m_colors.m_plot_lines;
			style.Colors[ImGuiCol_PlotLinesHovered] = m_colors.m_plot_lines_hovered;
			style.Colors[ImGuiCol_PlotHistogram] = m_colors.m_plot_histogram;
			style.Colors[ImGuiCol_PlotHistogramHovered] = m_colors.m_plot_histogram_hovered;
			style.Colors[ImGuiCol_TextSelectedBg] = m_colors.m_text_selected_bg;
			style.Colors[ImGuiCol_DragDropTarget] = m_colors.m_drag_drop_target;
			style.Colors[ImGuiCol_NavHighlight] = m_colors.m_nav_highlight;
			style.Colors[ImGuiCol_NavWindowingHighlight] = m_colors.m_nav_windowing_highlight;
			style.Colors[ImGuiCol_NavWindowingDimBg] = m_colors.m_nav_windowing_dim_bg;
			style.Colors[ImGuiCol_ModalWindowDimBg] = m_colors.m_modal_window_dim_bg;
		}
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

		void set_theme(const ui_theme& theme)
		{
			theme.apply();
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