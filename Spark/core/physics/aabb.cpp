#include "aabb.hpp"

namespace Spark {
AABB::AABB(const glm::vec3& center, f32 radius)
	: m_min(center - glm::vec3(radius))
	, m_max(center + glm::vec3(radius)) {}
bool AABB::Intersects(const AABB& other) const { return m_min.x <= other.m_max.x && m_max.x >= other.m_min.x && m_min.y <= other.m_max.y && m_max.y >= other.m_min.y && m_min.z <= other.m_max.z && m_max.z >= other.m_min.z; }
bool AABB::Contains(const glm::vec3& point) const { return point.x >= m_min.x && point.x <= m_max.x && point.y >= m_min.y && point.y <= m_max.y && point.z >= m_min.z && point.z <= m_max.z; }
bool AABB::Contains(const AABB& other) const { return m_min.x <= other.m_min.x && m_max.x >= other.m_max.x && m_min.y <= other.m_min.y && m_max.y >= other.m_max.y && m_min.z <= other.m_min.z && m_max.z >= other.m_max.z; }
bool AABB::Contains(const glm::vec3& min, const glm::vec3& max) const { return m_min.x <= min.x && m_max.x >= max.x && m_min.y <= min.y && m_max.y >= max.y && m_min.z <= min.z && m_max.z >= max.z; }
} // namespace Spark