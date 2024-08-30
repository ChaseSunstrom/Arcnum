#ifndef SPARK_TRANSFORM_HPP
#define SPARK_TRANSFORM_HPP

#include <core/pch.hpp>
#include <include/glm/matrix.hpp>

namespace Spark {
class Transform {
  public:
	Transform(const glm::vec3& position = glm::vec3(1), const glm::vec3& rotation = glm::vec3(1), f32 scale = 1);
	Transform(const glm::mat4& transform);
	bool operator==(const Transform& other) const;
	const glm::mat4& GetTransform() const { return m_transform; }
	void Move(const glm::vec3& position);
	void Rotate(const glm::vec3& rotation);
	void Scale(f32 scale);

  private:
	glm::mat4 m_transform;
};
} // namespace Spark

#endif