#ifndef SPARK_PHYSICS_HPP
#	define SPARK_PHSYICS_HPP

#	include <core/ecs/components/rigid_body_component.hpp>
#	include <core/ecs/query.hpp>
#	include <core/ecs/system.hpp>
#	include <core/pch.hpp>

namespace Spark {

class PhysicsSystem : public ISystem {
  public:
	PhysicsSystem(f32 time_step, const glm::vec3& gravity = glm::vec3(0.0f, -9.81f, 0.0f), i32 iterations = 4);
	~PhysicsSystem() override = default;

	void Start() override;
	void Update() override;
	void Shutdown() override;

	void SetGravity(const glm::vec3& gravity) { m_gravity = gravity; }
	void SetTimeStep(f32 time_step) { m_time_step = time_step; }
	void SetIterations(i32 iterations) { m_iterations = iterations; }

  private:
	void IntegrateForces(RigidBodyComponent& rb, f32 dt);
	void IntegrateVelocities(RigidBodyComponent& rb, f32 dt);
	void DetectCollisions();
	void ResolveCollisions();

  private:
	Query<RigidBodyComponent> m_rigid_bodies;
	glm::vec3 m_gravity;
	f32 m_time_step;
	i32 m_iterations;
};

#endif