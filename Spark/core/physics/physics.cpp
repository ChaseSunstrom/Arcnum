#include "physics.hpp"

namespace Spark {
	PhysicsSystem::PhysicsSystem(EventHandler& event_handler, f32 time_step, const glm::vec3& gravity, i32 iterations)
		: System(event_handler)
		, m_gravity(gravity)
		, m_time_step(time_step)
		, m_iterations(iterations) {}

	void PhysicsSystem::Start() {}

	void PhysicsSystem::Update(f32 delta_time) {
		f32 dt = m_time_step / static_cast<f32>(m_iterations);

		for (i32 i = 0; i < m_iterations; ++i) {
			// Apply forces (including gravity)
			for (auto& rb : m_rigid_bodies) {
				IntegrateForces(rb, dt);
			}

			DetectCollisions();
			ResolveCollisions();

			// Update positions
			for (auto& rb : m_rigid_bodies) {
				IntegrateVelocities(rb, dt);
			}
		}
	}

	void PhysicsSystem::Shutdown() {
		// Clean up any resources used by the physics system
		m_rigid_bodies.clear();
	}

	void PhysicsSystem::IntegrateForces(RigidBodyComponent& rb, f32 dt) {
		if (rb.rb->GetInvMass() == 0.0f)
			return; // Static objects don't move

		// Apply gravity
		rb.rb->GetVelocity() += m_gravity * dt;

		// Apply forces
		rb.rb->GetVelocity() += rb.rb->GetForce() * rb.rb->GetInvMass() * dt;

		// Apply torque
		rb.rb->GetAngularVelocity() += rb.rb->GetInvInertiaTensor() * rb.rb->GetTorque() * dt;

		// Clear forces and torques
		rb.rb->GetForce()  = glm::vec3(0.0f);
		rb.rb->GetTorque() = glm::vec3(0.0f);
	}

	void PhysicsSystem::IntegrateVelocities(RigidBodyComponent& rb, f32 dt) {
		if (rb.rb->GetInvMass() == 0.0f)
			return; // Static objects don't move

		// Update position
		rb.rb->GetPosition() += rb.rb->GetVelocity() * dt;

		// Update orientation
		glm::quat spin(0, rb.rb->GetAngularVelocity() * 0.5f * dt);
		rb.rb->GetOrientation() = glm::normalize(spin * rb.rb->GetOrientation());

		// Update the shape's orientation if it's an OBB
		if (rb.rb->GetShape()->GetType() == ShapeType::OBB) {
			OBBShape* obb_shape = static_cast<OBBShape*>(rb.rb->GetShape());
			obb_shape->SetOrientation(rb.rb->GetOrientation());
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
} // namespace Spark