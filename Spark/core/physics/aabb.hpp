#ifndef SPARK_AABB_HPP
#define SPARK_AABB_HPP

#include <core/pch.hpp>
#include <include/glm/glm.hpp>

namespace Spark {
class AABB {
  public:
	AABB() = default;
	AABB(const glm::vec3& min, const glm::vec3& max)
		: m_min(min)
		, m_max(max){};
	AABB(const glm::vec3& center, f32 radius);
	~AABB() = default;

	const glm::vec3& GetMin() const { return m_min; }
	const glm::vec3& GetMax() const { return m_max; }

	bool Intersects(const AABB& other) const;
	bool Contains(const glm::vec3& point) const;
	bool Contains(const AABB& other) const;
	bool Contains(const glm::vec3& min, const glm::vec3& max) const;
  private:
	glm::vec3 m_min;
	glm::vec3 m_max;
};
} // namespace Spark

#endif