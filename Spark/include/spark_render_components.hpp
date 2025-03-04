#ifndef SPARK_RENDER_COMPONENTS_HPP
#define SPARK_RENDER_COMPONENTS_HPP

#include "spark_pch.hpp"
#include "spark_mesh.hpp"
#include "spark_shader.hpp"
#include "spark_math.hpp"

namespace spark
{
    // Generic material system that doesn't hardcode properties
    class Material
    {
    public:
        // Store any type of uniform value
        using UniformValue = std::variant<
            int,
            float,
            math::Vec2,
            math::Vec3,
            math::Vec4,
            math::Mat4,
            std::vector<math::Mat4>, // For instance data
            std::vector<float>,      // For custom array data
            std::vector<int>         // For custom array data
        >;

        // Set any uniform value
        template<typename T>
        void SetUniform(const std::string& name, const T& value)
        {
            m_uniforms[name] = value;
        }

        // Apply all uniforms to a shader
        void ApplyToShader(IShaderProgram* shader) const
        {
            if (!shader) return;

            for (const auto& [name, value] : m_uniforms)
            {
                std::visit([&](const auto& v) {
                    ApplyUniform(shader, name, v);
                }, value);
            }
        }

    private:
        // Helper functions to apply different uniform types
        static void ApplyUniform(IShaderProgram* shader, const std::string& name, int value)
        {
            shader->SetUniformInt(name, value);
        }

        static void ApplyUniform(IShaderProgram* shader, const std::string& name, float value)
        {
            shader->SetUniformFloat(name, value);
        }

        static void ApplyUniform(IShaderProgram* shader, const std::string& name, const math::Vec2& value)
        {
            shader->SetUniformVec2(name, value);
        }

        static void ApplyUniform(IShaderProgram* shader, const std::string& name, const math::Vec3& value)
        {
            shader->SetUniformVec3(name, value);
        }

        static void ApplyUniform(IShaderProgram* shader, const std::string& name, const math::Vec4& value)
        {
            shader->SetUniformVec4(name, value);
        }

        static void ApplyUniform(IShaderProgram* shader, const std::string& name, const math::Mat4& value)
        {
            shader->SetUniformMat4(name, value);
        }

        static void ApplyUniform(IShaderProgram* shader, const std::string& name, const std::vector<math::Mat4>& values)
        {
            for (size_t i = 0; i < values.size(); ++i)
            {
                shader->SetUniformMat4(name + "[" + std::to_string(i) + "]", values[i]);
            }
        }

        static void ApplyUniform(IShaderProgram* shader, const std::string& name, const std::vector<float>& values)
        {
            for (size_t i = 0; i < values.size(); ++i)
            {
                shader->SetUniformFloat(name + "[" + std::to_string(i) + "]", values[i]);
            }
        }

        static void ApplyUniform(IShaderProgram* shader, const std::string& name, const std::vector<int>& values)
        {
            for (size_t i = 0; i < values.size(); ++i)
            {
                shader->SetUniformInt(name + "[" + std::to_string(i) + "]", values[i]);
            }
        }

    private:
        std::unordered_map<std::string, UniformValue> m_uniforms;
    };

    // Unified renderable component that contains all necessary rendering data
    struct RenderableComponent
    {
        IMesh* mesh = nullptr;
        IShaderProgram* shader = nullptr;
        std::shared_ptr<Material> material = std::make_shared<Material>();
        bool visible = true;
        bool depth_test = true;
        bool blending = false;
        bool wireframe = false;
        i32 draw_mode = 0; // e.g., GL_TRIANGLES

        // Optional instance data
        std::vector<math::Mat4> instance_transforms;
        bool is_instanced() const { return !instance_transforms.empty(); }
    };
}

#endif // SPARK_RENDER_COMPONENTS_HPP