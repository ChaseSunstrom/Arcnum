#include "rigid_body.hpp"

namespace Spark {
RigidBody::RigidBody(std::unique_ptr<CollisionShape> shape, f32 mass)
	: m_shape(std::move(shape))
	, m_mass(mass)
	, m_inv_mass(1.0f / mass)
	, m_position(0.0f)
	, m_velocity(0.0f)
	, m_angular_velocity(0.0f)
	, m_force(0.0f)
	, m_torque(0.0f)
	, m_gravity_affected(true) {
	m_inertia     = m_shape->CalculateInertiaTensor(mass);
	m_inv_inertia = glm::inverse(m_inertia);
}

void RigidBody::ApplyForce(const glm::vec3& force) {
	m_force += force;
}

void RigidBody::ApplyTorque(const glm::vec3& torque) {
	m_torque += torque;
}

void RigidBody::Update(f32 delta_time) {

}
} // namespace Spark