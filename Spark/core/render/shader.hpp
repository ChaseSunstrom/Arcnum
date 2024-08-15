#ifndef SPARK_SHADER_HPP
#define SPARK_SHADER_HPP

#include <core/pch.hpp>
#include <core/system/manager.hpp>

namespace Spark
{
	// Note: We have made the constructors protected, but we want to make sure that they are stored in the Registry<T>
    class Shader
    {
    public:
        friend class Manager<Shader>;
        virtual ~Shader() = default;
        virtual const std::filesystem::path& GetPath() const = 0;
        virtual void Compile() = 0;
    protected:
        Shader(const std::filesystem::path& path) : m_path(path) {}
    protected:
        std::filesystem::path m_path;
    };

    class VertexShader : public Shader
    {
    public:
        VertexShader(const std::filesystem::path& path) : Shader(path) {}
        const std::filesystem::path& GetPath() const override {
            return m_path;
        }
        void Compile() override = 0;
    };

    class FragmentShader : public Shader
    {
    public:
        FragmentShader(const std::filesystem::path& path) : Shader(path) {}
        const std::filesystem::path& GetPath() const override {
            return m_path;
        }
        void Compile() override = 0;
    };

    class ComputeShader : public Shader
    {
    public:
        ComputeShader(const std::filesystem::path& path) : Shader(path) {}
        const std::filesystem::path& GetPath() const override {
            return m_path;
        }
        void Compile() override = 0;
    };

    class GeometryShader : public Shader
    {
    public:
        GeometryShader(const std::filesystem::path& path) : Shader(path) {}
        const std::filesystem::path& GetPath() const override {
            return m_path;
        }
        void Compile() override = 0;
    };

    class TesselationShader : public Shader
    {
    public:
        TesselationShader(const std::filesystem::path& path) : Shader(path) {}
        const std::filesystem::path& GetPath() const override {
            return m_path;
        }
        void Compile() override = 0;
    };

    // Basically just a combination of a VertexShader and a FragmentShader
    class IRenderShader
    {
    public:
        virtual ~IRenderShader() = default;
    protected:
        IRenderShader() = default;
    };
}


#endif