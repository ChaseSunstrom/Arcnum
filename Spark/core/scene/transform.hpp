#ifndef SPARK_TRANSFORM_HPP
#define SPARK_TRANSFORM_HPP

#include <core/math/math.hpp>
#include <core/pch.hpp>

namespace Spark {
	class Transform {
	  public:
		Transform(const _MATH Vec3& position = _MATH Vec3(0), const _MATH Vec3& rotation = _MATH Vec3(0), const _MATH Vec3& scale = _MATH Vec3(1))
			: m_position(position)
			, m_rotation(_MATH EulerToQuaternion(rotation))
			, m_scale(scale) {
			UpdateTransform();
		}

		Transform(const _MATH Vec3& position, const _MATH Quat& rotation, const _MATH Vec3& scale)
			: m_position(position)
			, m_rotation(rotation)
			, m_scale(scale) {
			UpdateTransform();
		}

		void Move(const _MATH Vec3& delta) {
			m_position += delta;
			UpdateTransform();
		}

		void Move(f32 x, f32 y, f32 z) {
			m_position += _MATH Vec3(x, y, z);
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
			m_rotation = _MATH EulerToQuaternion(_MATH Vec3(x, 0, 0)) * m_rotation;
			UpdateTransform();
		}

		void RotateY(f32 y) {
			m_rotation = _MATH EulerToQuaternion(_MATH Vec3(0, y, 0)) * m_rotation;
			UpdateTransform();
		}

		void RotateZ(f32 z) {
			m_rotation = _MATH EulerToQuaternion(_MATH Vec3(0, 0, z)) * m_rotation;
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
			m_rotation = _MATH EulerToQuaternion(_MATH Vec3(x, y, z)) * m_rotation;
			UpdateTransform();
		}

		void Rotate(const _MATH Vec3& euler) {
			m_rotation = _MATH EulerToQuaternion(euler) * m_rotation;
			UpdateTransform();
		}

		void Rotate(const _MATH Vec3& axis, f32 angle) {
			m_rotation = _MATH Quat(_MATH Rotate(_MATH Mat4(1.0f), angle, axis)) * m_rotation;
			UpdateTransform();
		}

		void Scale(const _MATH Vec3& scale) {
			m_scale *= scale;
			UpdateTransform();
		}

		void SetPosition(const _MATH Vec3& position) {
			m_position = position;
			UpdateTransform();
		}

		void SetRotation(const _MATH Vec3& euler) {
			m_rotation = _MATH EulerToQuaternion(euler);
			UpdateTransform();
		}

		void SetRotation(const _MATH Quat& rotation) {
			m_rotation = rotation;
			UpdateTransform();
		}

		void SetScale(const _MATH Vec3& scale) {
			m_scale = scale;
			UpdateTransform();
		}

		_MATH Vec3 GetPosition() const { return m_position; }
		_MATH Quat GetRotation() const { return m_rotation; }
		_MATH Vec3 GetScale() const { return m_scale; }
		_MATH Vec3 GetEulerRotation() const { return _MATH QuaternionToEuler(m_rotation); }

		const _MATH Mat4& GetTransform() const { return m_transform; }

	  private:
		void UpdateTransform() { m_transform = _MATH Translate(_MATH Mat4(1.0f), m_position) * _MATH Mat4Cast(m_rotation) * _MATH Scale(_MATH Mat4(1.0f), m_scale); }

		_MATH Vec3 m_position;
		_MATH Quat m_rotation;
		_MATH Vec3 m_scale;
		_MATH Mat4 m_transform;
	};
} // namespace Spark

#endif