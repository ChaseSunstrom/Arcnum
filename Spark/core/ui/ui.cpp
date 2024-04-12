#include "ui.hpp"
#include "IMGUI/imgui_impl_vulkan.h"
#include "../window/window_manager.hpp"

namespace spark
{
	void ui_manager::on_update()
	{
		auto& _vulkan_window = engine::get<vulkan_window>();

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		draw_components();

		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), _vulkan_window.get_window_data().m_command_buffers[_vulkan_window.get_window_data().m_current_frame]);
	}
}