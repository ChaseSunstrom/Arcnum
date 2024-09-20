#ifndef SPARK_RIGID_BODY_HPP
#define SPARK_RIGID_BODY_HPP
#include <core/pch.hpp>
#include "collision_shape.hpp"

namespace Spark {

	class RigidBody {
	  public:
		RigidBody(UniquePtr<CollisionShape> shape, f32 mass);

		void ApplyForce(const _MATH Vec3& force);
		void ApplyTorque(const _MATH Vec3& torque);
		void Update(f32 delta_time);

		// Getters and setters
		const _MATH Vec3&     GetPosition() const { return m_position; }
		_MATH Vec3&           GetPosition() { return m_position; }
		void                  SetPosition(const _MATH Vec3& position) { m_position = position; }
		const _MATH Vec3&     GetVelocity() const { return m_velocity; }
		_MATH Vec3&           GetVelocity() { return m_velocity; }
		void                  SetVelocity(const _MATH Vec3& velocity) { m_velocity = velocity; }
		const _MATH Vec3&     GetAngularVelocity() const { return m_angular_velocity; }
		_MATH Vec3&           GetAngularVelocity() { return m_angular_velocity; }
		void                  SetAngularVelocity(const _MATH Vec3& angular_velocity) { m_angular_velocity = angular_velocity; }
		const _MATH Quat&     GetOrientation() const { return m_orientation; }
		_MATH Quat&           GetOrientation() { return m_orientation; }
		void                  SetOrientation(const _MATH Quat& orientation) { m_orientation = orientation; }
		f32                   GetMass() const { return m_mass; }
		f32                   GetInvMass() const { return m_inv_mass; }
		const _MATH Mat3&     GetInertia() const { return m_inertia; }
		const _MATH Mat3&     GetInvInertiaTensor() const { return m_inv_inertia; }
		const _MATH Vec3&     GetForce() const { return m_force; }
		_MATH Vec3&           GetForce() { return m_force; }
		const _MATH Vec3&     GetTorque() const { return m_torque; }
		_MATH Vec3&           GetTorque() { return m_torque; }
		bool                  IsGravityAffected() const { return m_gravity_affected; }
		void                  SetGravityAffected(bool affected) { m_gravity_affected = affected; }
		const CollisionShape* GetShape() const { return m_shape.Get(); }
		CollisionShape*       GetShape() { return m_shape.Get(); }
		f32                   GetRestitution() const { return m_restitution; }
		f32                   GetStaticFriction() const { return m_static_friction; }
		f32                   GetDynamicFriction() const { return m_dynamic_friction; }

	  private:
		UniquePtr<CollisionShape> m_shape;
		f32                       m_mass;
		f32                       m_inv_mass;
		f32                       m_restitution;
		f32                       m_static_friction;
		f32                       m_dynamic_friction;
		_MATH Vec3                m_position;
		_MATH Vec3                m_velocity;
		_MATH Vec3                m_angular_velocity;
		_MATH Quat                m_orientation;
		_MATH Mat3                m_inertia;
		_MATH Mat3                m_inv_inertia;
		_MATH Vec3                m_force;
		_MATH Vec3                m_torque;
		bool                      m_gravity_affected;
	};

} // namespace Spark
#endif // SPARK_RIGID_BODY_HPP