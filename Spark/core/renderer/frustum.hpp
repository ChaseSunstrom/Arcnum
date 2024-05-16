#ifndef SPARK_FRUSTUM_HPP
#define SPARK_FRUSTUM_HPP

#include "../ecs/ecs.hpp"
#include "../spark.hpp"

namespace Spark
{
struct Plane
{
    Plane() = default;

    Plane(const Math::vec3 &normal, f32 distance) : m_normal(Math::normalize(normal)), m_distance(distance)
    {
    }

    f32 get_signed_distance(const Math::vec3 &point) const
    {
        return Math::dot(m_normal, point) + m_distance;
    }

    Math::vec3 m_normal;

    f32 m_distance;
};

struct Frustum
{
    Frustum() = default;

    Frustum(const Math::mat4 &pv_matrix);

    void update(const Math::mat4 &view_projection_matrix);

    bool is_inside(const Math::vec3 &center, f32 size) const;

    std::array<Math::vec3, 8> get_corners() const;

    std::array<Plane, 6> m_planes;

    Math::mat4 m_view_projection;
};
} // namespace Spark

#endif