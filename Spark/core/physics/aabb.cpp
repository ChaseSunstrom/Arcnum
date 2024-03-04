#include "aabb.hpp"

namespace spark
{
	aabb::aabb(const std::vector<vertex>& vertices)
	{
        if (vertices.empty()) 
        {
            m_min = math::vec3(std::numeric_limits<float>::max());
            m_max = math::vec3(-std::numeric_limits<float>::max());
        }

        else 
        {
            m_min = vertices[0].m_position;
            m_max = vertices[0].m_position;

            for (const auto& v : vertices)
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

    bool aabb::check_collision(const aabb& other) const
    {
        return (m_min.x <= other.m_max.x && m_max.x >= other.m_min.x) &&
            (m_min.y <= other.m_max.y && m_max.y >= other.m_min.y) &&
            (m_min.z <= other.m_max.z && m_max.z >= other.m_min.z);
    }
}