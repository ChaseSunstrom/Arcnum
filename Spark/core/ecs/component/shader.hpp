#ifndef SPARK_SHADER_HPP
#define SPARK_SHADER_HPP

#include "../../logging/log.hpp"
#include "../../spark.hpp"

namespace Spark
{
enum class ShaderType
{
    UNKNOWN = 0,
    VERTEX,
    FRAGMENT,
    GEOMETRY,
    COMPUTE,
    TESS_CONTROL,
    TESS_EVAL
};

struct VulkanShaderWrapper
{
    // Forwards the shader type and vk_shader_module
    VulkanShaderWrapper(ShaderType type, internal::VkShaderModule module);

    internal::VkPipelineShaderStageCreateInfo m_pipeline_shader{};

    std::optional<internal::VkPipelineVertexInputStateCreateInfo> m_vertex_input;

  private:
    void create_vertex_input();
};

struct DirectXShaderWrapper
{
};

struct MetalShaderWrapper
{
};

struct ShaderWrapper
{
    ShaderWrapper() = default;

    ShaderType get_shader_type_from_extension(const std::string &file_extension);

    void create_vulkan_shader(const std::filesystem::path &shader_path);

    void create_metal_shader();

    void create_directx_shader();

    ShaderType m_shader_type;

    std::variant<std::unique_ptr<VulkanShaderWrapper>, std::unique_ptr<DirectXShaderWrapper>,
                 std::unique_ptr<MetalShaderWrapper>>
        m_shader_variant;
};

class Shader
{
  public:
    Shader(const std::filesystem::path &shader_path)
    {
        create_shader(shader_path);
    }

    ShaderType get_shader_type() const
    {
        return m_shader->m_shader_type;
    }

  private:
    void create_shader(const std::filesystem::path &shader_path);

  private:
    std::unique_ptr<ShaderWrapper> m_shader = std::make_unique<ShaderWrapper>();
};

class ShaderManager
{
  public:
    static ShaderManager &get()
    {
        static ShaderManager instance;
        return instance;
    }

    Shader &create(const std::filesystem::path &shader_path);

    Shader &get_shader(const std::string &path);

  private:
    ShaderManager() = default;

    ~ShaderManager() = default;

  private:
    std::unordered_map<std::string, std::unique_ptr<Shader>> m_shaders =
        std::unordered_map<std::string, std::unique_ptr<Shader>>();
};
} // namespace Spark

#endif