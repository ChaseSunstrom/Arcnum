// SPARK_GL_SHADER_PROGRAM_CPP
#include "spark_pch.hpp"
#include "opengl/spark_gl_shader.hpp"
#include <GL/glew.h>


namespace spark::opengl
{

    GLShaderProgram::GLShaderProgram()
    {
        m_program_id = glCreateProgram();
    }

    GLShaderProgram::~GLShaderProgram()
    {
        glDeleteProgram(m_program_id);
    }

    void GLShaderProgram::Bind() const
    {
        glUseProgram(m_program_id);
    }

    void GLShaderProgram::Unbind() const
    {
        glUseProgram(0);
    }

    i32 GLShaderProgram::GetUniformLocation(const std::string& name) const
    {
        if (m_uniform_location_cache.find(name) != m_uniform_location_cache.end())
        {
            return m_uniform_location_cache[name];
        }

        i32 location = glGetUniformLocation(m_program_id, name.c_str());
        if (location == -1)
        {
            Logln(LogLevel::WARN) << "uniform '" << name << "' does not exist or is not used in shader!\n";
        }
        m_uniform_location_cache[name] = location;
        return location;
    }

    u32 GLShaderProgram::CompileShader(u32 type, const std::string& source)
    {
        u32 shader = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        i32 success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char info_log[512];
            glGetShaderInfoLog(shader, 512, nullptr, info_log);
            Logln(LogLevel::ERROR) << "Shader compilation failed: " << info_log << "\n";
        }
        return shader;
    }

    void GLShaderProgram::AddShader(ShaderStageType stage, const std::string& source)
    {
        u32 shader_type = 0;
        switch (stage)
        {
        case ShaderStageType::VERTEX:
            shader_type = GL_VERTEX_SHADER;
            break;
        case ShaderStageType::FRAGMENT:
            shader_type = GL_FRAGMENT_SHADER;
            break;
        case ShaderStageType::GEOMETRY:
            shader_type = GL_GEOMETRY_SHADER;
            break;
        case ShaderStageType::COMPUTE:
            shader_type = GL_COMPUTE_SHADER;
            break;
        default:
            break;
        }

        u32 shader = CompileShader(shader_type, source);
        glAttachShader(m_program_id, shader);
        m_shader_ids.push_back(shader);
    }

    bool GLShaderProgram::Link()
    {
        glLinkProgram(m_program_id);

        i32 success;
        glGetProgramiv(m_program_id, GL_LINK_STATUS, &success);
        if (!success)
        {
            char info_log[512];
            glGetProgramInfoLog(m_program_id, 512, nullptr, info_log);
            Logln(LogLevel::ERROR) << "Shader program linking failed: " << info_log << "\n";
            return false;
        }

        // Once linked, detach and delete shader objects.
        for (u32 shader : m_shader_ids)
        {
            glDetachShader(m_program_id, shader);
            glDeleteShader(shader);
        }
        m_shader_ids.clear();
        return true;
    }

    void GLShaderProgram::SetUniformInt(const std::string& name, i32 value)
    {
        Bind();
        glUniform1i(GetUniformLocation(name), value);
    }

    void GLShaderProgram::SetUniformFloat(const std::string& name, f32 value)
    {
        Bind();
        glUniform1f(GetUniformLocation(name), value);
    }

    void GLShaderProgram::SetUniformVec2(const std::string& name, const math::Vec2& value)
    {
        Bind();
        glUniform2f(GetUniformLocation(name), value.x, value.y);
    }

    void GLShaderProgram::SetUniformVec3(const std::string& name, const math::Vec3& value)
    {
        Bind();
        glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
    }

    void GLShaderProgram::SetUniformVec4(const std::string& name, const math::Vec4& value)
    {
        Bind();
        glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
    }

    void GLShaderProgram::SetUniformMat4(const std::string& name, const math::Mat4& value)
    {
        Bind();
        // Assumes that math::Mat4::Data() returns a poi32er to a contiguous block of f32s.
        glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, value.Data());
    }

} // namespace spark::opengl
