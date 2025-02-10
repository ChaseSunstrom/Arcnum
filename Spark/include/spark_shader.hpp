#ifndef SPARK_SHADER_PROGRAM_HPP
#define SPARK_SHADER_PROGRAM_HPP

#include "spark_pch.hpp"
#include "spark_math.hpp"

namespace spark
{
    // Enum for specifying shader stage types.
    enum class ShaderStageType
    {
        VERTEX,
        FRAGMENT,
        GEOMETRY,
        COMPUTE
    };

    // Abstract interface for a shader program.
    class SPARK_API IShaderProgram
    {
    public:
        virtual ~IShaderProgram() = default;

        // Binds the shader program for use.
        virtual void Bind() const = 0;

        // Unbinds the shader program.
        virtual void Unbind() const = 0;

        // Adds a shader stage to the program.
        virtual void AddShader(ShaderStageType stage, const std::string& source) = 0;

        // Links the shader program. Returns true on success.
        virtual bool Link() = 0;

        // Uniform setters.
        virtual void SetUniformInt(const std::string& name, int value) = 0;
        virtual void SetUniformFloat(const std::string& name, float value) = 0;
        virtual void SetUniformVec2(const std::string& name, const math::Vec2& value) = 0;
        virtual void SetUniformVec3(const std::string& name, const math::Vec3& value) = 0;
        virtual void SetUniformVec4(const std::string& name, const math::Vec4& value) = 0;
        virtual void SetUniformMat4(const std::string& name, const math::Mat4& value) = 0;
    };
}

#endif // SPARK_SHADER_PROGRAM_HPP
