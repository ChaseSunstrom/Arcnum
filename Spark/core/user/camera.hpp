#ifndef SPARK_CAMERA_HPP
#define SPARK_CAMERA_HPP

#include "../spark.hpp"
#include "../renderer/frustum.hpp"

namespace spark
{
	struct camera
	{
		camera(const math::vec3& position = math::vec3(0.0f, 0.0f, 3.0f), 
			   const math::vec3& direction = math::vec3(0.0f, 0.0f, -1.0f), 
			   f32 fov = 90.0f, 
			   f32 aspect_ratio = 1, 
			   f32 near_plane = 0.1f, 
			   f32 far_plane = 100.0f) :
			m_position(position), 
			m_direction(direction), 
			m_fov(fov), 
			m_aspect_ratio(aspect_ratio), 
			m_near_plane(near_plane), 
			m_far_plane(far_plane)
		{
			m_frustum = std::make_unique<frustum>(get_view_projection_matrix());
		}

		math::mat4 get_view_matrix() const
		{
			return math::lookAt(m_position, m_position + m_direction, math::vec3(0.0f, 1.0f, 0.0f));
		}

		math::mat4 get_projection_matrix() const
		{
			return math::perspective(math::radians(m_fov), m_aspect_ratio, m_near_plane, m_far_plane);
		}

		math::mat4 get_view_projection_matrix() const
		{
			return get_projection_matrix() * get_view_matrix();
		}

		bool operator!=(const camera& other) const
		{
			return !(m_position == other.m_position && m_direction == other.m_direction && m_fov == other.m_fov && m_aspect_ratio == other.m_aspect_ratio && m_near_plane == other.m_near_plane && m_far_plane == other.m_far_plane);
		}

		math::vec3 m_position;
		math::vec3 m_direction;
		f32 m_fov;
		f32 m_aspect_ratio;
		f32 m_near_plane;
		f32 m_far_plane;

		std::unique_ptr<frustum> m_frustum;
	};
}

#endif