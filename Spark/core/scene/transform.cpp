#include "transform.hpp"
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace Spark
{
	Transform::Transform(const glm::vec3& position, const glm::vec3& rotation, f32 scale)
	{
		m_transform = glm::translate(glm::mat4(1.0f), position) 
				    * glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1, 0, 0)) 
			        * glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0, 1, 0)) 
			        * glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0, 0, 1)) 
			        * glm::scale(glm::mat4(1.0f), glm::vec3(scale));
	}

	Transform::Transform(const glm::mat4& transform)
	{
		m_transform = transform;
	}

	void Transform::Move(const glm::vec3& position)
	{
		m_transform = glm::translate(m_transform, position);
	}

	void Transform::Rotate(const glm::vec3& rotation)
	{
		m_transform = glm::rotate(m_transform, rotation.x, glm::vec3(1, 0, 0));
		m_transform = glm::rotate(m_transform, rotation.y, glm::vec3(0, 1, 0));
		m_transform = glm::rotate(m_transform, rotation.z, glm::vec3(0, 0, 1));
	}

	void Transform::Scale(f32 scale)
	{
		m_transform = glm::scale(m_transform, glm::vec3(scale));
	}
}