#include "ui.hpp"
#include "IMGUI/imgui_impl_vulkan.h"
#include "../window/window_manager.hpp"

namespace spark
{
	void ui_manager::on_update()
	{
		auto& vk_window = engine::get<vulkan_window>();

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		draw_components();

		ImGui::Render();

		ImGui_ImplVulkan_RenderDrawData(
				ImGui::GetDrawData(),
				vk_window.get_window_data().m_command_buffers[vk_window.get_window_data().m_current_frame],
			    vk_window.get_window_data().m_graphics_pipeline);
	}
}