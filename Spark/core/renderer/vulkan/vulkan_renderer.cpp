#include "vulkan_renderer.hpp"
#include "../../ecs/ecs.hpp"
#include "../../window/vulkan/vulkan_mesh.hpp"

namespace Spark
{
void VulkanRenderer::render(VulkanWindowData &window_data)
{
    for (auto &[mesh_name, material_map] : m_renderables)
    {
        for (auto &[material_name, transforms_ptr] : material_map)
        {
            auto &mesh = Engine::get<MeshManager>().get_mesh(mesh_name);

            if (is_current_api(API::VULKAN))
            {
                auto &vulkan_mesh = dynamic_cast<VulkanMesh &>(mesh);

                Internal::VkBuffer vertex_buffers[] = {vulkan_mesh.m_vertex_buffer};
                Internal::VkDeviceSize offsets[] = {0};

                vulkan_mesh.update_uniform_buffers();

                vkCmdBindVertexBuffers(window_data.m_command_buffers[window_data.m_current_frame], 0, 1, vertex_buffers,
                                       offsets);

                if (!vulkan_mesh.m_indices.empty())
                {
                    vkCmdBindIndexBuffer(window_data.m_command_buffers[window_data.m_current_frame],
                                         vulkan_mesh.m_index_buffer, 0, Internal::VK_INDEX_TYPE_UINT32);

                    vkCmdBindDescriptorSets(window_data.m_command_buffers[window_data.m_current_frame],
                                            Internal::VK_PIPELINE_BIND_POINT_GRAPHICS, window_data.m_pipeline_layout, 0,
                                            1, &window_data.m_descriptor_sets[window_data.m_current_frame], 0, nullptr);

                    vkCmdDrawIndexed(window_data.m_command_buffers[window_data.m_current_frame],
                                     vulkan_mesh.m_indices.size(), transforms_ptr->m_data.size(), 0, 0, 0);
                }

                else
                {
                    vkCmdBindDescriptorSets(window_data.m_command_buffers[window_data.m_current_frame],
                                            Internal::VK_PIPELINE_BIND_POINT_GRAPHICS, window_data.m_pipeline_layout, 0,
                                            1, &window_data.m_descriptor_sets[window_data.m_current_frame], 0, nullptr);

                    vkCmdDraw(window_data.m_command_buffers[window_data.m_current_frame], vulkan_mesh.m_vertices.size(),
                              transforms_ptr->m_data.size(), 0, 0);
                }
            }
        }
    }
}

void VulkanRenderer::toggle_wireframe_mode()
{

}

void VulkanRenderer::render_debugging_tools()
{
}
} // namespace Spark