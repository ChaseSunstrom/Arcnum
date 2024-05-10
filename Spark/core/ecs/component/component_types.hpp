#ifndef SPARK_COMPONENT_TYPES_HPP
#define SPARK_COMPONENT_TYPES_HPP

#include "../../spark.hpp"
#include "../../window/vulkan/vulkan_mesh.hpp"
#include "../../app/api.hpp"
#include "mesh_manager.hpp"
#include "material_manager.hpp"
#include "texture_manager.hpp"
#include "shader.hpp"


namespace Spark
{

struct Transform
{
    Transform(const math::vec3 &position = math::vec3(0.0f), const math::vec3 &rotation = math::vec3(0.0f),
              const math::vec3 &scale = math::vec3(1.0f))
    {
        math::mat4 mat = math::mat4(1.0f); // Start with identity matrix

        // Apply translation, rotation, and scaling
        mat = math::translate(mat, position);
        mat = math::rotate(mat, math::radians(rotation.z), math::vec3(0.0f, 0.0f, 1.0f));
        mat = math::rotate(mat, math::radians(rotation.y), math::vec3(0.0f, 1.0f, 0.0f));
        mat = math::rotate(mat, math::radians(rotation.x), math::vec3(1.0f, 0.0f, 0.0f));
        mat = math::scale(mat, scale);

        m_transform = mat;
    }

    void update_matrix(const math::vec3 &position, const math::vec3 &rotation, const math::vec3 &scale)
    {
        m_transform = math::mat4(1.0f); // Reset to identity matrix
        m_transform = math::translate(m_transform, position);
        m_transform = math::rotate(m_transform, math::radians(rotation.z), math::vec3(0.0f, 0.0f, 1.0f));
        m_transform = math::rotate(m_transform, math::radians(rotation.y), math::vec3(0.0f, 1.0f, 0.0f));
        m_transform = math::rotate(m_transform, math::radians(rotation.x), math::vec3(1.0f, 0.0f, 0.0f));
        m_transform = math::scale(m_transform, scale);
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

    math::mat4 m_transform;
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