#ifndef SPARK_RIGID_BODY_HPP
#define SPARK_RIGID_BODY_HPP
#include "collision_shape.hpp"
#include <core/pch.hpp>
#include <include/glm/glm.hpp>
#include <include/glm/gtx/quaternion.hpp>

namespace Spark {

class RigidBody {
  public:
	RigidBody(std::unique_ptr<CollisionShape> shape, f32 mass);

	void ApplyForce(const glm::vec3& force);
	void ApplyTorque(const glm::vec3& torque);
	void Update(f32 delta_time);

	// Getters and setters
	const glm::vec3& GetPosition() const { return m_position; }
	glm::vec3& GetPosition() { return m_position; }
	void SetPosition(const glm::vec3& position) { m_position = position; }
	const glm::vec3& GetVelocity() const { return m_velocity; }
	glm::vec3& GetVelocity() { return m_velocity; }
	void SetVelocity(const glm::vec3& velocity) { m_velocity = velocity; }
	const glm::vec3& GetAngularVelocity() const { return m_angular_velocity; }
	glm::vec3& GetAngularVelocity() { return m_angular_velocity; }
	void SetAngularVelocity(const glm::vec3& angular_velocity) { m_angular_velocity = angular_velocity; }
	const glm::quat& GetOrientation() const { return m_orientation; }
	glm::quat& GetOrientation() { return m_orientation; }
	void SetOrientation(const glm::quat& orientation) { m_orientation = orientation; }
	f32 GetMass() const { return m_mass; }
	f32 GetInvMass() const { return m_inv_mass; }
	const glm::mat3& GetInertia() const { return m_inertia; }
	const glm::mat3& GetInvInertiaTensor() const { return m_inv_inertia; }
	const glm::vec3& GetForce() const { return m_force; }
	glm::vec3& GetForce() { return m_force; }
	const glm::vec3& GetTorque() const { return m_torque; }
	glm::vec3& GetTorque() { return m_torque; }
	bool IsGravityAffected() const { return m_gravity_affected; }
	void SetGravityAffected(bool affected) { m_gravity_affected = affected; }
	const CollisionShape* GetShape() const { return m_shape.get(); }
	CollisionShape* GetShape() { return m_shape.get(); }
	f32 GetRestitution() const { return m_restitution; }
	f32 GetStaticFriction() const { return m_static_friction; }
	f32 GetDynamicFriction() const { return m_dynamic_friction; }

  private:
	std::unique_ptr<CollisionShape> m_shape;
	f32 m_mass;
	f32 m_inv_mass;
	f32 m_restitution;
	f32 m_static_friction;
	f32 m_dynamic_friction;
	glm::vec3 m_position;
	glm::vec3 m_velocity;
	glm::vec3 m_angular_velocity;
	glm::quat m_orientation;
	glm::mat3 m_inertia;
	glm::mat3 m_inv_inertia;
	glm::vec3 m_force;
	glm::vec3 m_torque;
	bool m_gravity_affected;
};

} // namespace Spark
#endif // SPARK_RIGID_BODY_HPP