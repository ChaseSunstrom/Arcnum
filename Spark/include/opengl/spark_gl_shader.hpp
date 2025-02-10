#ifndef SPARK_GL_SHADER_PROGRAM_HPP
#define SPARK_GL_SHADER_PROGRAM_HPP

#include "spark_shader.hpp"

namespace spark::opengl
{
    class SPARK_API GLShaderProgram : public IShaderProgram
    {
    public:
        GLShaderProgram();
        ~GLShaderProgram() override;

        void Bind() const override;
        void Unbind() const override;
        void AddShader(ShaderStageType stage, const std::string& source) override;
        bool Link() override;
        void SetUniformInt(const std::string& name, i32 value) override;
        void SetUniformFloat(const std::string& name, f32 value) override;
        void SetUniformVec2(const std::string& name, const math::Vec2& value) override;
        void SetUniformVec3(const std::string& name, const math::Vec3& value) override;
        void SetUniformVec4(const std::string& name, const math::Vec4& value) override;
        void SetUniformMat4(const std::string& name, const math::Mat4& value) override;

    private:
        u32 m_program_id;
        std::vector<u32> m_shader_ids;
        mutable std::unordered_map<std::string, i32> m_uniform_location_cache;

        // Returns the location of a uniform variable (with caching).
        i32 GetUniformLocation(const std::string& name) const;

        // Compiles a shader from source code.
        u32 CompileShader(u32 type, const std::string& source);
    };
}

#endif // SPARK_GL_SHADER_PROGRAM_HPP
