#ifndef SPARK_RIGID_BODY_HPP
#define SPARK_RIGID_BODY_HPP

#include "../ecs/ecs.hpp"
#include "aabb.hpp"

namespace Spark
{
struct RigidBody
{
    RigidBody(f64 mass = 1, Transform *transform = nullptr, math::vec3 velocity = math::vec3(0.0f),
              std::unique_ptr<AABB> aabb = nullptr)
        : m_mass(mass), m_transform(transform), m_velocity(velocity), m_aabb(std::move(aabb))
    {
    }

    math::vec3 m_velocity;
    std::unique_ptr<AABB> m_aabb;
    f64 m_mass;
    Transform *m_transform;
};

using rigid_body_component = RigidBody;
} // namespace Spark

#endif