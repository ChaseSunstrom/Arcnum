#ifndef SPARK_RIGID_BODY_HPP
#define SPARK_RIGID_BODY_HPP

#include <core/pch.hpp>
#include <glm/glm.hpp>
#include "collision_shape.hpp"

namespace Spark {

class RigidBody {
  public:
	RigidBody(std::unique_ptr<CollisionShape> shape, f32 mass);

	void ApplyForce(const glm::vec3& force);
	void ApplyTorque(const glm::vec3& torque);
	void Update(f32 delta_time);

	// Getters and setters
	const glm::vec3& GetPosition() const { return m_position; }
	void SetPosition(const glm::vec3& position) { m_position = position; }
	const glm::vec3& GetVelocity() const { return m_velocity; }
	void SetVelocity(const glm::vec3& velocity) { m_velocity = velocity; }
	f32 GetMass() const { return m_mass; }

  private:
	std::unique_ptr<CollisionShape> m_shape;
	f32 m_mass;
	f32 m_inv_mass;
	glm::vec3 m_position;
	glm::vec3 m_velocity;
	glm::vec3 m_angular_velocity;
	glm::mat3 m_inertia;
	glm::mat3 m_inv_inertia;
	glm::vec3 m_force;
	glm::vec3 m_torque;
	bool m_gravity_affected;
};

} // namespace Spark

#endif // SPARK_RIGID_BODY_HPP