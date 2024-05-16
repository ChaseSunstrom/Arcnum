#include "aabb.hpp"

namespace Spark
{
AABB::AABB(const std::vector<Vertex> &vertices)
{
    if (vertices.empty())
    {
        m_min = Math::vec3(std::numeric_limits<f32>::max());
        m_max = Math::vec3(-std::numeric_limits<f32>::max());
    }

    else
    {
        m_min = vertices[0].m_position;
        m_max = vertices[0].m_position;

        for (const auto &v : vertices)
        {
            m_min.x = std::min(m_min.x, v.m_position.x);
            m_min.y = std::min(m_min.y, v.m_position.y);
            m_min.z = std::min(m_min.z, v.m_position.z);

            m_max.x = std::max(m_max.x, v.m_position.x);
            m_max.y = std::max(m_max.y, v.m_position.y);
            m_max.z = std::max(m_max.z, v.m_position.z);
        }
    }
}

bool AABB::check_collision(const AABB &other) const
{
    return (m_min.x <= other.m_max.x && m_max.x >= other.m_min.x) &&
           (m_min.y <= other.m_max.y && m_max.y >= other.m_min.y) &&
           (m_min.z <= other.m_max.z && m_max.z >= other.m_min.z);
}
} // namespace Spark