#include "shader.hpp"
#include "../../util/file.hpp"
#include "../../window/window_manager.hpp"
#include <d3dcompiler.h>

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

VulkanShaderWrapper::VulkanShaderWrapper(ShaderType type, Internal::VkShaderModule module) : ShaderWrapper(type)
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
    default:
        vk_shader_type = static_cast<Internal::VkShaderStageFlagBits>(0);
        break;
    }

    m_pipeline_shader.stage = vk_shader_type;
    m_pipeline_shader.module = module;
    m_pipeline_shader.pName = "main";
}

void VulkanShaderWrapper::create_shader(const std::filesystem::path &shader_code)
{
    auto &vk_window = Engine::get<VulkanWindow>();

    std::string code = read_file(shader_code);
    Internal::VkShaderModuleCreateInfo create_info = {};
    create_info.sType = Internal::VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t *>(code.data());

    Internal::VkShaderModule shader_module;
    if (vkCreateShaderModule(vk_window.get_window_data().m_device, &create_info, nullptr, &shader_module) !=
        Internal::VK_SUCCESS)
    {
        SPARK_ERROR("[SHADER] Failed to create shader module");
    }
}

void MetalShaderWrapper::create_shader(const std::filesystem::path& shader_path)
{

}

void DirectXShaderWrapper::create_shader(const std::filesystem::path &shader_path)
{
    std::wstring wide_shader_path(shader_path.wstring());
    // Use the existing constructor logic to create shaders.

    Microsoft::WRL::ComPtr<ID3DBlob> shader_blob;
    Microsoft::WRL::ComPtr<ID3DBlob> error_blob;
    HRESULT hr;

    if (m_shader_type == ShaderType::VERTEX)
    {
        hr = D3DCompileFromFile(shader_path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", 0, 0,
                                &shader_blob, &error_blob);
        if (FAILED(hr))
        {
            if (error_blob)
                SPARK_ERROR("Vertex Shader Compilation Error: " << (char *)error_blob->GetBufferPointer());
            return;
        }
        m_device->CreateVertexShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), nullptr,
                                   &m_vertex_shader);
    }
    else if (m_shader_type == ShaderType::FRAGMENT)
    {
        hr = D3DCompileFromFile(shader_path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", 0, 0,
                                &shader_blob, &error_blob);
        if (FAILED(hr))
        {
            if (error_blob)
                SPARK_ERROR("Pixel Shader Compilation Error: " << (char *)error_blob->GetBufferPointer());
            return;
        }
        m_device->CreatePixelShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), nullptr,
                                  &m_pixel_shader);
    }

    m_shader_blob = shader_blob;
}

void Shader::create_shader(const std::filesystem::path &shader_path)
{
    std::string file_extension = get_file_extension(shader_path);
    ShaderType type = m_shader->get_shader_type_from_extension(file_extension);

    switch (get_current_api())
    {
    case API::DIRECTX: {
        m_shader = std::make_unique<DirectXShaderWrapper>(type, DirectXWindow::get().get_window_data().m_device.Get());
        m_shader->create_shader(shader_path);
        break;
    }
    case API::VULKAN: {
        Internal::VkShaderModule dummy_module = {};
        m_shader = std::make_unique<VulkanShaderWrapper>(type, dummy_module);
        m_shader->create_shader(shader_path);
        break;
    }
    case API::METAL: {
        m_shader = std::make_unique<MetalShaderWrapper>();
        m_shader->create_shader(shader_path);
        break;
    }
    default:
        SPARK_ERROR("[SHADER] Unsupported API");
        break;
    }
}

Shader::Shader(const std::filesystem::path &shader_path)
{
    create_shader(shader_path);
}

Shader &ShaderManager::create(const std::filesystem::path &shader_path)
{
    m_shaders[shader_path.string()] = std::make_unique<Shader>(shader_path);
    return *m_shaders[shader_path.string()];
}

Shader &ShaderManager::get_shader(const std::string &path)
{
    return *m_shaders[path];
}
} // namespace Spark
