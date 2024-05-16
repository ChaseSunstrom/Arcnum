#include "shader.hpp"

#include "../../util/file.hpp"
#include "../../window/window_manager.hpp"

namespace Spark
{
ShaderType ShaderWrapper::get_shader_type_from_extension(const std::string &file_extension)
{
    if (file_extension == ".vert")
    {
        return ShaderType::VERTEX;
    }
    else if (file_extension == ".frag")
    {
        return ShaderType::FRAGMENT;
    }
    else if (file_extension == ".geom")
    {
        return ShaderType::GEOMETRY;
    }
    else if (file_extension == ".comp")
    {
        return ShaderType::COMPUTE;
    }
    else if (file_extension == ".tesc")
    {
        return ShaderType::TESS_CONTROL;
    }
    else if (file_extension == ".tese")
    {
        return ShaderType::TESS_EVAL;
    }
    else
    {
        SPARK_ERROR("[SHADER] Shader type not supported");
        return ShaderType::UNKNOWN;
    }
}

VulkanShaderWrapper::VulkanShaderWrapper(ShaderType type, Internal::VkShaderModule module)
{
    m_pipeline_shader.sType = Internal::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

    Internal::VkShaderStageFlagBits vk_shader_type;
    switch (type)
    {
    case ShaderType::VERTEX:
        vk_shader_type = Internal::VK_SHADER_STAGE_VERTEX_BIT;
        break;
    case ShaderType::FRAGMENT:
        vk_shader_type = Internal::VK_SHADER_STAGE_FRAGMENT_BIT;
        break;
    case ShaderType::COMPUTE:
        vk_shader_type = Internal::VK_SHADER_STAGE_COMPUTE_BIT;
        break;
    case ShaderType::GEOMETRY:
        vk_shader_type = Internal::VK_SHADER_STAGE_GEOMETRY_BIT;
        break;
    case ShaderType::TESS_CONTROL:
        vk_shader_type = Internal::VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        break;
    case ShaderType::TESS_EVAL:
        vk_shader_type = Internal::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        break;
    }

    m_pipeline_shader.stage = vk_shader_type;
    m_pipeline_shader.module = module;
    m_pipeline_shader.pName = "main";
}

void ShaderWrapper::create_vulkan_shader(const std::filesystem::path &shader_code)
{
    auto &vk_window = Engine::get<VulkanWindow>();

    std::string code = read_file(shader_code);
    Internal::VkShaderModuleCreateInfo create_info = {};
    create_info.sType = Internal::VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const u32 *>(code.data());

    Internal::VkShaderModule shader_module;
    if (vkCreateShaderModule(vk_window.get_window_data().m_device, &create_info, nullptr, &shader_module) !=
        Internal::VK_SUCCESS)
    {
        SPARK_ERROR("[SHADER] Failed to create shader module");
    }

    m_shader_variant = std::make_unique<VulkanShaderWrapper>(m_shader_type, shader_module);
}

void Shader::create_shader(const std::filesystem::path &shader_path)
{
    std::string file_extension = get_file_extension(shader_path);
    ShaderType type = m_shader->get_shader_type_from_extension(file_extension);

    m_shader->m_shader_type = type;

    switch (get_current_api())
    {
    case API::DIRECTX: {
        // Create DirectX shader
        break;
    }
    case API::VULKAN: {
        m_shader->create_vulkan_shader(shader_path);
        break;
    }
    case API::METAL: {
        // Create Metal shader
        break;
    }
    }
}

Shader &ShaderManager::create(const std::filesystem::path &shader_path)
{
    std::unique_ptr<Shader> shader = std::make_unique<Shader>(shader_path);
    m_shaders[shader_path.string()] = std::move(shader);
    return *m_shaders[shader_path.string()];
}

Shader &ShaderManager::get_shader(const std::string &path)
{
    return *m_shaders[path];
}
} // namespace Spark