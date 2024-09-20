#include "rigid_body.hpp"

namespace Spark {
	RigidBody::RigidBody(UniquePtr<CollisionShape> shape, f32 mass)
		: m_shape(Move(shape))
		, m_mass(mass)
		, m_inv_mass(1.0f / mass)
		, m_position(0.0f)
		, m_velocity(0.0f)
		, m_angular_velocity(0.0f)
		, m_force(0.0f)
		, m_torque(0.0f)
		, m_gravity_affected(true) {
		m_inertia     = m_shape->CalculateInertiaTensor(mass);
		m_inv_inertia = _MATH Inverse(m_inertia);
	}

	void RigidBody::ApplyForce(const _MATH Vec3& force) { m_force += force; }

	void RigidBody::ApplyTorque(const _MATH Vec3& torque) { m_torque += torque; }

	void RigidBody::Update(f32 delta_time) {}
} // namespace Spark