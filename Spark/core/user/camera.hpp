#ifndef SPARK_CAMERA_HPP
#define SPARK_CAMERA_HPP

#include "../spark.hpp"

namespace spark
{
	struct camera_component
	{
		camera_component() = default;

		camera_component(const math::vec3& position, const math::vec3& direction, float32_t fov, float32_t aspect_ratio, float32_t near_plane, float32_t far_plane) :
			m_position(position), m_direction(direction), m_fov(fov), m_aspect_ratio(aspect_ratio), m_near_plane(near_plane), m_far_plane(far_plane) {}

		math::mat4 get_view_matrix() const
		{
			return math::lookAt(m_position, m_position + m_direction, math::vec3(0.0f, 1.0f, 0.0f));
		}

		math::mat4 get_projection_matrix() const
		{
			return math::perspective(math::radians(m_fov), m_aspect_ratio, m_near_plane, m_far_plane);
		}

		bool operator!=(const camera_component& other) const
		{
			return !(m_position == other.m_position && m_direction == other.m_direction && m_fov == other.m_fov && m_aspect_ratio == other.m_aspect_ratio && m_near_plane == other.m_near_plane && m_far_plane == other.m_far_plane);
		}

		math::vec3 m_position;
		math::vec3 m_direction;
		float32_t m_fov;
		float32_t m_aspect_ratio;
		float32_t m_near_plane;
		float32_t m_far_plane;
	};
}

#endif