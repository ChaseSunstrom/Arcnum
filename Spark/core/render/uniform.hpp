#ifndef SPARK_UNIFORM_HPP
#define SPARK_UNIFORM_HPP

#include <core/pch.hpp>
#include <include/glm/glm.hpp>
#include "shader.hpp"

namespace Spark
{
    // Forward declaration
    class Shader;

    class IUniform
    {
    public:
        virtual ~IUniform() = default;
        virtual void Apply(Shader& shader) const = 0;
        virtual std::string GetName() const = 0;
    };

    template <typename T>
    class Uniform : public IUniform
    {
    public:
        Uniform(const std::string& name, const T& value)
            : m_name(name), m_value(value) {}

        void Apply(Shader& shader) const override;
        std::string GetName() const override { return m_name; }

        void SetValue(const T& value) { m_value = value; }
        const T& GetValue() const { return m_value; }
    private:
        std::string m_name;
        T m_value;
    };

    class UniformBuffer 
    {
    public:
        UniformBuffer(const std::string& name, size_t size, u32 binding_point)
            : m_name(name), m_size(size), m_binding_point(binding_point) {
            glGenBuffers(1, &m_ubo);
            glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
            glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, m_ubo);
        }

        ~UniformBuffer() {
            glDeleteBuffers(1, &m_ubo);
        }

        void SetData(const void* data, size_t size, size_t offset = 0) {
            glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
            glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }

        const std::string& GetName() const { return m_name; }
        u32 GetBindingPoint() const { return m_binding_point; }

    private:
        std::string m_name;
        u32 m_ubo;
        size_t m_size;
        u32 m_binding_point;
    };

    // Specialized uniform types
    using UniformVec2 = Uniform<glm::vec2>;
    using UniformVec3 = Uniform<glm::vec3>;
    using UniformVec4 = Uniform<glm::vec4>;
    using UniformMat2 = Uniform<glm::mat2>;
    using UniformMat3 = Uniform<glm::mat3>;
    using UniformMat4 = Uniform<glm::mat4>;
    using UniformI32 = Uniform<i32>;
    using UniformI64 = Uniform<i64>;
    using UniformF32 = Uniform<f32>;
    using UniformF64 = Uniform<f64>;
    using UniformBool = Uniform<bool>;

    // Variant to hold any type of uniform
    using UniformVariant = std::variant<
        UniformVec2, UniformVec3, UniformVec4,
        UniformMat2, UniformMat3, UniformMat4,
        UniformI32, UniformI64, UniformF32, UniformF64, UniformBool
    >;

    class UniformSet
    {
    public:
        template <typename T>
        void SetUniform(const std::string& name, const T& value)
        {
            auto it = std::find_if(m_uniforms.begin(), m_uniforms.end(),
                [&name](const UniformVariant& u) {
                    return std::visit([&name](const auto& uniform) {
                        return uniform.GetName() == name;
                        }, u);
                });

            if (it != m_uniforms.end()) {
                *it = Uniform<T>(name, value);
            }
            else {
                m_uniforms.emplace_back(Uniform<T>(name, value));
            }
        }

        void ApplyUniforms(Shader& shader) const
        {
            for (const auto& uniform : m_uniforms) {
                std::visit([&shader](const auto& u) { u.Apply(shader); }, uniform);
            }
        }

    private:
        std::vector<UniformVariant> m_uniforms;
    };

    template <>
    void Uniform<glm::vec2>::Apply(Shader& shader) const
    {
        shader.SetVec2(m_name, m_value);
    }

    template <>
    void Uniform<glm::vec3>::Apply(Shader& shader) const
    {
        shader.SetVec3(m_name, m_value);
    }

    template <>
    void Uniform<glm::vec4>::Apply(Shader& shader) const
    {
        shader.SetVec4(m_name, m_value);
    }

    template <>
    void Uniform<glm::mat2>::Apply(Shader& shader) const
    {
        shader.SetMat2(m_name, m_value);
    }

    template <>
    void Uniform<glm::mat3>::Apply(Shader& shader) const
    {
        shader.SetMat3(m_name, m_value);
    }

    template <>
    void Uniform<glm::mat4>::Apply(Shader& shader) const
    {
        shader.SetMat4(m_name, m_value);
    }

    template <>
    void Uniform<i32>::Apply(Shader& shader) const
    {
        shader.SetI32(m_name, m_value);
    }

    template <>
    void Uniform<i64>::Apply(Shader& shader) const
    {
        shader.SetI64(m_name, m_value);
    }

    template <>
    void Uniform<f32>::Apply(Shader& shader) const
    {
        shader.SetF32(m_name, m_value);
    }

    template <>
    void Uniform<f64>::Apply(Shader& shader) const
    {
        shader.SetF64(m_name, m_value);
    }

    template <>
    void Uniform<bool>::Apply(Shader& shader) const
    {
        shader.SetBool(m_name, m_value);
    }
}

#endif