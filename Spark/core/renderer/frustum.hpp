#ifndef SPARK_FRUSTUM_HPP
#define SPARK_FRUSTUM_HPP

#include "../spark.hpp"
#include "../ecs/ecs.hpp"

namespace spark
{
	struct Plane
	{
		Plane() = default;

		Plane(const math::vec3& normal, f32 distance) :
				m_normal(math::normalize(normal)), m_distance(distance) { }

		f32 get_signed_distance(const math::vec3& point) const
		{
			return math::dot(m_normal, point) + m_distance;
		}

		math::vec3 m_normal;

		f32 m_distance;
	};

	struct Frustum
	{
		Frustum() = default;

		Frustum(const math::mat4& pv_matrix);

		void update(const math::mat4& view_projection_matrix);

		bool is_inside(const math::vec3& center, f32 size) const;

		std::array<math::vec3, 8> get_corners() const;

		std::array<Plane, 6> m_planes;

		math::mat4 m_view_projection;
	};
}

#endif