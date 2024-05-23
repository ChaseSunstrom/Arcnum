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

struct ShaderWrapper
{
    ShaderWrapper(ShaderType type = ShaderType::UNKNOWN) : m_shader_type(type)
    {}

    ShaderType get_shader_type_from_extension(const std::string &file_extension);

    virtual void create_shader(const std::filesystem::path &shader_path) = 0;

    ShaderType m_shader_type;
};

struct VulkanShaderWrapper : ShaderWrapper
{
    VulkanShaderWrapper(ShaderType type, Internal::VkShaderModule module);

    void create_shader(const std::filesystem::path &shader_code) override;

    Internal::VkPipelineShaderStageCreateInfo m_pipeline_shader{};
    std::optional<Internal::VkPipelineVertexInputStateCreateInfo> m_vertex_input;

  private:
    void create_vertex_input();
};

struct DirectXShaderWrapper : ShaderWrapper
{
    DirectXShaderWrapper(ShaderType type, ID3D11Device *device) : ShaderWrapper(type), m_device(device)
    {}

    void create_shader(const std::filesystem::path &shader_path) override;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertex_shader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixel_shader;
    ID3D11Device *m_device;
    Microsoft::WRL::ComPtr<ID3DBlob> m_shader_blob;
};

struct MetalShaderWrapper : ShaderWrapper
{
    void create_shader(const std::filesystem::path &shader_path) override;
};

class Shader
{
  public:
    Shader(const std::filesystem::path &shader_path);

    ShaderType get_shader_type() const
    {
        return m_shader->m_shader_type;
    }

    ShaderWrapper &get_shader()
    {
        return *m_shader;
    }

  private:
    void create_shader(const std::filesystem::path &shader_path);

  private:
    std::unique_ptr<ShaderWrapper> m_shader;
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
    std::unordered_map<std::string, std::unique_ptr<Shader>> m_shaders;
};
} // namespace Spark

#endif // SPARK_SHADER_HPP
