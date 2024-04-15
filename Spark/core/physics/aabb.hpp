#ifndef SPARK_AABB_HPP
#define SPARK_AABB_HPP

#include "../spark.hpp"
#include "../ecs/component/component_types.hpp"

namespace spark
{
	class aabb
	{
	public:
		aabb(math::vec3 min, math::vec3 max) :
				m_min(min), m_max(max) { }

		aabb(const std::vector <vertex>& vertices);

		bool check_collision(const aabb& other) const;

	private:
		math::vec3 m_min;

		math::vec3 m_max;
	};
}

#endif