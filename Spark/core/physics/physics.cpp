#include "physics.hpp"

namespace Spark
{
PhysicsSystem::PhysicsSystem(f32 time_step, const glm::vec3& gravity, i32 iterations)
	: m_gravity(gravity)
	, m_time_step(time_step)
	, m_iterations(iterations) {}

void PhysicsSystem::Start() {
}

void PhysicsSystem::Update() {
	f32 dt = m_time_step / static_cast<f32>(m_iterations);

	for (i32 i = 0; i < m_iterations; ++i) {
		// Apply forces (including gravity)
		for (auto& rb : m_rigid_bodies) {
			IntegrateForces(*rb, dt);
		}

		DetectCollisions();
		ResolveCollisions();

		// Update positions
		for (auto& rb : m_rigid_bodies) {
			IntegrateVelocities(*rb, dt);
		}
	}
}

void PhysicsSystem::Shutdown() {
	// Clean up any resources used by the physics system
	m_rigid_bodies.clear();
}

void PhysicsSystem::IntegrateForces(RigidBodyComponent& rb, f32 dt) {
	if (rb.GetInvMass() == 0.0f)
		return; // Static objects don't move

	// Apply gravity
	rb.GetVelocity() += m_gravity * dt;

	// Apply forces
	rb.GetVelocity() += rb.GetForce() * rb.GetInvMass() * dt;

	// Apply torque
	rb.GetAngularVelocity() += rb.GetInvInertiaTensor() * rb.GetTorque() * dt;

	// Clear forces and torques
	rb.GetForce()  = glm::vec3(0.0f);
	rb.GetTorque() = glm::vec3(0.0f);
}

void PhysicsSystem::IntegrateVelocities(RigidBodyComponent& rb, f32 dt) {
	if (rb.GetInvMass() == 0.0f)
		return; // Static objects don't move

	// Update position
	rb.GetPosition() += rb.GetVelocity() * dt;

	// Update orientation
	glm::quat spin(0, rb.GetAngularVelocity() * 0.5f * dt);
	rb.GetOrientation() = glm::normalize(spin * rb.GetOrientation());

	// Update the shape's orientation if it's an OBB
	if (rb.GetShape()->GetType() == ShapeType::OBB) {
		OBBShape* obb_shape = static_cast<OBBShape*>(rb.GetShape().get());
		obb_shape->SetOrientation(rb.GetOrientation());
	}
}

void PhysicsSystem::DetectCollisions() {
	// Implement broad phase and narrow phase collision detection
	// This is a placeholder for where you'd implement your collision detection algorithm
}

void PhysicsSystem::ResolveCollisions() {
	// Implement collision resolution
	// This is a placeholder for where you'd implement your collision resolution algorithm
}
}