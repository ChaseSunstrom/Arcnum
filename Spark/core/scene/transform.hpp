#ifndef SPARK_TRANSFORM_HPP
#define SPARK_TRANSFORM_HPP

#include <core/math/math.hpp>
#include <core/pch.hpp>

namespace Spark {
	class Transform {
	  public:
		Transform(const Math::Vec3& position = Math::Vec3(0), const Math::Vec3& rotation = Math::Vec3(0), const Math::Vec3& scale = Math::Vec3(1))
			: m_position(position)
			, m_rotation(Math::EulerToQuaternion(rotation))
			, m_scale(scale) {
			UpdateTransform();
		}

		Transform(const Math::Vec3& position, const Math::Quat& rotation, const Math::Vec3& scale)
			: m_position(position)
			, m_rotation(rotation)
			, m_scale(scale) {
			UpdateTransform();
		}

		void Move(const Math::Vec3& delta) {
			m_position += delta;
			UpdateTransform();
		}

		void Move(f32 x, f32 y, f32 z) {
			m_position += Math::Vec3(x, y, z);
			UpdateTransform();
		}

		void MoveX(f32 x) {
			m_position.x += x;
			UpdateTransform();
		}

		void MoveY(f32 y) {
			m_position.y += y;
			UpdateTransform();
		}

		void MoveZ(f32 z) {
			m_position.z += z;
			UpdateTransform();
		}

		void RotateX(f32 x) {
			m_rotation = Math::EulerToQuaternion(Math::Vec3(x, 0, 0)) * m_rotation;
			UpdateTransform();
		}

		void RotateY(f32 y) {
			m_rotation = Math::EulerToQuaternion(Math::Vec3(0, y, 0)) * m_rotation;
			UpdateTransform();
		}

		void RotateZ(f32 z) {
			m_rotation = Math::EulerToQuaternion(Math::Vec3(0, 0, z)) * m_rotation;
			UpdateTransform();
		}

		void ScaleX(f32 x) {
			m_scale.x *= x;
			UpdateTransform();
		}

		void ScaleY(f32 y) {
			m_scale.y *= y;
			UpdateTransform();
		}

		void ScaleZ(f32 z) {
			m_scale.z *= z;
			UpdateTransform();
		}

		void Rotate(f32 x, f32 y, f32 z) {
			m_rotation = Math::EulerToQuaternion(Math::Vec3(x, y, z)) * m_rotation;
			UpdateTransform();
		}

		void Rotate(const Math::Vec3& euler) {
			m_rotation = Math::EulerToQuaternion(euler) * m_rotation;
			UpdateTransform();
		}

		void Rotate(const Math::Vec3& axis, f32 angle) {
			m_rotation = Math::Quat(Math::Rotate(Math::Mat4(1.0f), angle, axis)) * m_rotation;
			UpdateTransform();
		}

		void Scale(const Math::Vec3& scale) {
			m_scale *= scale;
			UpdateTransform();
		}

		void SetPosition(const Math::Vec3& position) {
			m_position = position;
			UpdateTransform();
		}

		void SetRotation(const Math::Vec3& euler) {
			m_rotation = Math::EulerToQuaternion(euler);
			UpdateTransform();
		}

		void SetRotation(const Math::Quat& rotation) {
			m_rotation = rotation;
			UpdateTransform();
		}

		void SetScale(const Math::Vec3& scale) {
			m_scale = scale;
			UpdateTransform();
		}

		Math::Vec3 GetPosition() const { return m_position; }
		Math::Quat GetRotation() const { return m_rotation; }
		Math::Vec3 GetScale() const { return m_scale; }
		Math::Vec3 GetEulerRotation() const { return Math::QuaternionToEuler(m_rotation); }

		const Math::Mat4& GetTransform() const { return m_transform; }

	  private:
		void UpdateTransform() { m_transform = Math::Translate(Math::Mat4(1.0f), m_position) * Math::Mat4Cast(m_rotation) * Math::Scale(Math::Mat4(1.0f), m_scale); }

		Math::Vec3 m_position;
		Math::Quat m_rotation;
		Math::Vec3 m_scale;
		Math::Mat4 m_transform;
	};
} // namespace Spark

#endif