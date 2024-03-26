#ifndef SPARK_FRUSTUM_HPP
#define SPARK_FRUSTUM_HPP

#include "../spark.hpp"
#include "../ecs/ecs.hpp"

namespace spark
{
	struct plane
	{
		plane() = default;
		plane(const math::vec3& normal, float32_t distance) :
			m_normal(math::normalize(normal)), m_distance(distance) {}

		float32_t get_signed_distance(const math::vec3& point) const
		{
			return math::dot(m_normal, point) + m_distance;
		}

		math::vec3 m_normal;
		float32_t m_distance;
	};

	struct frustum
	{
		frustum() = default;
		frustum(const math::mat4& pv_matrix);
		void update(const math::mat4& view_projection_matrix);
		bool is_inside(const math::vec3& center, float32_t size) const;
		std::array<math::vec3, 8> get_corners() const;

		std::array<plane, 6> m_planes;
		math::mat4 m_view_projection;
	};
}

#endif