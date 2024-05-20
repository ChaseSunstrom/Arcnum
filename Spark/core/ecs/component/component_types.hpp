#ifndef SPARK_COMPONENT_TYPES_HPP
#define SPARK_COMPONENT_TYPES_HPP

#include "../../spark.hpp"
#include "../../app/api.hpp"
#include "shader.hpp"


namespace Spark
{

struct Transform
{
    Transform(const Math::vec3 &position = Math::vec3(0.0f), const Math::vec3 &rotation = Math::vec3(0.0f),
              const Math::vec3 &scale = Math::vec3(1.0f))
    {
        Math::mat4 mat = Math::mat4(1.0f); // Start with identity matrix

        // Apply translation, rotation, and scaling
        mat = Math::translate(mat, position);
        mat = Math::rotate(mat, Math::radians(rotation.z), Math::vec3(0.0f, 0.0f, 1.0f));
        mat = Math::rotate(mat, Math::radians(rotation.y), Math::vec3(0.0f, 1.0f, 0.0f));
        mat = Math::rotate(mat, Math::radians(rotation.x), Math::vec3(1.0f, 0.0f, 0.0f));
        mat = Math::scale(mat, scale);

        m_transform = mat;
    }

    void update_matrix(const Math::vec3 &position, const Math::vec3 &rotation, const Math::vec3 &scale)
    {
        m_transform = Math::mat4(1.0f); // Reset to identity matrix
        m_transform = Math::translate(m_transform, position);
        m_transform = Math::rotate(m_transform, Math::radians(rotation.z), Math::vec3(0.0f, 0.0f, 1.0f));
        m_transform = Math::rotate(m_transform, Math::radians(rotation.y), Math::vec3(0.0f, 1.0f, 0.0f));
        m_transform = Math::rotate(m_transform, Math::radians(rotation.x), Math::vec3(1.0f, 0.0f, 0.0f));
        m_transform = Math::scale(m_transform, scale);
    }

    Transform &operator*=(const Transform &rhs)
    {
        this->m_transform = this->m_transform * rhs.m_transform;
        return *this;
    }

    Transform operator*(const Transform &rhs) const
    {
        Transform result = *this; // Copy current transform
        result *= rhs;            // Apply multiplication
        return result;
    }

    Math::mat4 m_transform;
};

struct MeshComponent
{
    MeshComponent() = default;

    MeshComponent(const std::string &name) : m_mesh_name(name)
    {
    }

    ~MeshComponent() = default;

    std::string m_mesh_name = "";
};

struct MaterialComponent
{
    MaterialComponent() = default;

    MaterialComponent(const std::string &name) : m_material_name(name)
    {
    }

    ~MaterialComponent() = default;

    bool operator!=(const MaterialComponent &other) const
    {
        return m_material_name != other.m_material_name;
    }

    std::string m_material_name = "";
};

// Types
using TransformComponent = Transform;
} // namespace Spark

#endif