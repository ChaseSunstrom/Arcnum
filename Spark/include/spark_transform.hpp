#ifndef SPARK_TRANSFORM_HPP
#define SPARK_TRANSFORM_HPP

#include "spark_pch.hpp"
#include "spark_math.hpp"

namespace spark {
	class Transform {
	public:
		Transform(const math::Vec3& position = math::Vec3(0), const math::Vec3& rotation = math::Vec3(0), const math::Vec3& scale = math::Vec3(1))
			: m_position(position)
			, m_rotation(math::EulerToQuaternion(rotation))
			, m_scale(scale) {
			UpdateTransform();
		}

		Transform(const math::Mat4& mat)
			: m_transform(mat)
		{
			// Decompose the provided matrix into translation, rotation, and scale.
			math::DecomposeTransform(mat, m_position, m_rotation, m_scale);
		}


		Transform(const math::Vec3& position, const math::Quat& rotation, const math::Vec3& scale)
			: m_position(position)
			, m_rotation(rotation)
			, m_scale(scale) {
			UpdateTransform();
		}

		void Move(const math::Vec3& delta) {
			m_position += delta;
			UpdateTransform();
		}

		void Move(f32 x, f32 y, f32 z) {
			m_position += math::Vec3(x, y, z);
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
			m_rotation = math::EulerToQuaternion(math::Vec3(x, 0, 0)) * m_rotation;
			UpdateTransform();
		}

		void RotateY(f32 y) {
			m_rotation = math::EulerToQuaternion(math::Vec3(0, y, 0)) * m_rotation;
			UpdateTransform();
		}

		void RotateZ(f32 z) {
			m_rotation = math::EulerToQuaternion(math::Vec3(0, 0, z)) * m_rotation;
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
			m_rotation = math::EulerToQuaternion(math::Vec3(x, y, z)) * m_rotation;
			UpdateTransform();
		}

		void Rotate(const math::Vec3& euler) {
			m_rotation = math::EulerToQuaternion(euler) * m_rotation;
			UpdateTransform();
		}

		void Rotate(const math::Vec3& axis, f32 angle) {
			m_rotation = math::QuatFromAxisAngle(axis, angle) * m_rotation;
			UpdateTransform();
		}

		void Scale(const math::Vec3& scale) {
			m_scale *= scale;
			UpdateTransform();
		}

		void SetPosition(const math::Vec3& position) {
			m_position = position;
			UpdateTransform();
		}

		void SetRotation(const math::Vec3& euler) {
			m_rotation = math::EulerToQuaternion(euler);
			UpdateTransform();
		}

		void SetRotation(const math::Quat& rotation) {
			m_rotation = rotation;
			UpdateTransform();
		}

		void SetScale(const math::Vec3& scale) {
			m_scale = scale;
			UpdateTransform();
		}

		math::Vec3        GetPosition() const { return m_position; }
		math::Quat        GetRotation() const { return m_rotation; }
		math::Vec3        GetScale() const { return m_scale; }
		math::Vec3        GetEulerRotation() const { return math::QuaternionToEuler(m_rotation); }
		math::Mat4 GetTransform() const { return m_transform; }

		operator math::Mat4() const { return GetTransform(); }

	private:
		void UpdateTransform() { m_transform = math::Translate(math::Mat4(1.0f), m_position) * math::Mat4Cast(m_rotation) * math::Scale(math::Mat4(1.0f), m_scale); }

		math::Vec3 m_position;
		math::Quat m_rotation;
		math::Vec3 m_scale;
		math::Mat4 m_transform;
	};
} // namespace spark

#endif