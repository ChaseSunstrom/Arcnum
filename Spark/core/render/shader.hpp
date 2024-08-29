#ifndef SPARK_SHADER_HPP
#define SPARK_SHADER_HPP

#include <core/pch.hpp>
#include <core/system/manager.hpp>
#include <include/glm/glm.hpp>

namespace Spark
{
    enum class ShaderStage
    {
        Vertex,
        Fragment,
        Compute,
        Geometry,
        Tessellation
    };
    
    class IShader
    {
    public:
        virtual void Compile() = 0;
        virtual void Use() = 0;
        // Convenience methods for specific types
        virtual void SetVec2(const std::string& name, const glm::vec2& value) = 0;
        virtual void SetVec3(const std::string& name, const glm::vec3& value) = 0;
        virtual void SetVec4(const std::string& name, const glm::vec4& value) = 0;
        virtual void SetMat2(const std::string& name, const glm::mat2& value) = 0;
        virtual void SetMat3(const std::string& name, const glm::mat3& value) = 0;
        virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;
        virtual void SetI32(const std::string& name, const i32 value) = 0;
        virtual void SetI64(const std::string& name, const i64 value) = 0;
        virtual void SetF32(const std::string& name, const f32 value) = 0;
        virtual void SetF64(const std::string& name, const f64 value) = 0;
        virtual void SetBool(const std::string& name, const bool value) = 0;
        virtual void BindUniformBlock(const std::string& block_name, u32 binding_point) = 0;
        virtual u32 GetUniformBlockIndex(const std::string& block_name) = 0;
    };

    class Shader : public IShader
    {
    public:
        friend class Manager<Shader>;
        virtual ~Shader() = default;
        virtual const ShaderStage GetStage() const = 0;
        const std::filesystem::path& GetPath() const { return m_path; }
    protected:
        Shader(const std::filesystem::path& path) : m_path(path) {}
    protected:
        std::filesystem::path m_path;
    };

    class VertexShader : public Shader
    {
    public:
        friend class Manager<VertexShader>;
        const ShaderStage GetStage() const override { return ShaderStage::Vertex; }

    protected:
        VertexShader(const std::filesystem::path& path) : Shader(path) {}
    };

    class FragmentShader : public Shader
    {
    public:
        friend class Manager<FragmentShader>;
        const ShaderStage GetStage() const override { return ShaderStage::Fragment; }

    protected:
        FragmentShader(const std::filesystem::path& path) : Shader(path) {}
    };

    class ComputeShader : public Shader
    {
    public:
        friend class Manager<ComputeShader>;
        const ShaderStage GetStage() const override { return ShaderStage::Compute; }
        virtual void Dispatch(u32 num_groups_x, u32 num_groups_y, u32 num_groups_z) = 0;
        virtual void BindShaderStorageBlock(const std::string& block_name, u32 binding_point) = 0;

    protected:
        ComputeShader(const std::filesystem::path& path) : Shader(path) {}
    };

    class GeometryShader : public Shader
    {
    public:
        friend class Manager<GeometryShader>;
        const ShaderStage GetStage() const override { return ShaderStage::Geometry; }
    protected:
        GeometryShader(const std::filesystem::path& path) : Shader(path) {}
    };

    class TessellationShader : public Shader
    {
    public:
        friend class Manager<TessellationShader>;
        const ShaderStage GetStage() const override { return ShaderStage::Tessellation; }

    protected:
        TessellationShader(const std::filesystem::path& path) : Shader(path) {}
    };

    class RenderShader : public IShader
    {
    public:
        friend class Manager<RenderShader>;
        virtual ~RenderShader() = default;
        virtual void AddShaderStage(ShaderStage stage, const std::filesystem::path& path) = 0;
    protected:
        RenderShader(std::vector<std::pair<ShaderStage, const std::filesystem::path>> stage_paths) {
            for (const auto& [stage, path] : stage_paths)
                AddShaderStage(stage, path);
        }
        std::map<ShaderStage, std::filesystem::path> m_shader_paths;
    };

    
    template <typename T>
    concept IsShader = std::derived_from<T, Shader>;
}

#endif