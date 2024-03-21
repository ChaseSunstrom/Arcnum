#include "frustum.hpp"

namespace spark
{
	frustum::frustum(const math::mat4& pv_matrix)
	{
		update(pv_matrix);
	}

	void frustum::update(const math::mat4& pv_matrix)
	{
		for (int32_t i = 0; i < 3; i++)
		{
			m_planes[i] = glm::vec4(pv_matrix[0][3] + pv_matrix[0][i], pv_matrix[1][3] + pv_matrix[1][i], pv_matrix[2][3] + pv_matrix[2][i], pv_matrix[3][3] + pv_matrix[3][i]); // Left, bottom, near
			m_planes[i + 3] = glm::vec4(pv_matrix[0][3] - pv_matrix[0][i], pv_matrix[1][3] - pv_matrix[1][i], pv_matrix[2][3] - pv_matrix[2][i], pv_matrix[3][3] - pv_matrix[3][i]); // Right, top, far
		}
		// Normalize the planes
		for (auto& plane : m_planes)
		{
			float32_t length = math::length(glm::vec3(plane));
			plane /= length;
		}
	}

	bool frustum::is_inside(const math::vec3& center, float32_t size) const
	{
		glm::vec3 half_diagonal(size);
		for (const auto& plane : m_planes)
		{
			glm::vec3 normal(plane);
			float distance = plane.w + math::dot(normal, center);
			if (distance < -math::dot(normal, half_diagonal))
			{
				return false; // Box is completely outside this plane
			}
		}
		return true; // Box is inside all planes
	}
}