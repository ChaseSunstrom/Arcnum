#ifndef SPARK_TRANSFORM_HPP
#define SPARK_TRANSFORM_HPP

#include <core/math/math.hpp>
#include <core/pch.hpp>

namespace Spark {
	class Transform {
	  public:
		Transform(const Vec3& position = Vec3(0), const Vec3& rotation = Vec3(0), const Vec3& scale = Vec3(1))
			: m_position(position)
			, m_rotation(Math::EulerToQuaternion(rotation))
			, m_scale(scale) {
			UpdateTransform();
		}

		Transform(const Vec3& position, const Quat& rotation, const Vec3& scale)
			: m_position(position)
			, m_rotation(rotation)
			, m_scale(scale) {
			UpdateTransform();
		}

		void Move(const Vec3& delta) {
			m_position += delta;
			UpdateTransform();
		}

		void Move(f32 x, f32 y, f32 z) {
			m_position += Vec3(x, y, z);
			UpdateTransform();
		}

		void MoveX(f32 x) {
			m_position.X() += x;
			UpdateTransform();
		}

		void MoveY(f32 y) {
			m_position.Y() += y;
			UpdateTransform();
		}

		void MoveZ(f32 z) {
			m_position.Z() += z;
			UpdateTransform();
		}

		void RotateX(f32 x) {
			m_rotation = Math::EulerToQuaternion(Vec3(x, 0, 0)) * m_rotation;
			UpdateTransform();
		}

		void RotateY(f32 y) {
			m_rotation = Math::EulerToQuaternion(Vec3(0, y, 0)) * m_rotation;
			UpdateTransform();
		}

		void RotateZ(f32 z) {
			m_rotation = Math::EulerToQuaternion(Vec3(0, 0, z)) * m_rotation;
			UpdateTransform();
		}

		void ScaleX(f32 x) {
			m_scale.X() *= x;
			UpdateTransform();
		}

		void ScaleY(f32 y) {
			m_scale.Y() *= y;
			UpdateTransform();
		}

		void ScaleZ(f32 z) {
			m_scale.Z() *= z;
			UpdateTransform();
		}

		void Rotate(f32 x, f32 y, f32 z) {
			m_rotation = Math::EulerToQuaternion(Vec3(x, y, z)) * m_rotation;
			UpdateTransform();
		}

		void Rotate(const Vec3& euler) {
			m_rotation = Math::EulerToQuaternion(euler) * m_rotation;
			UpdateTransform();
		}

		void Rotate(const Vec3& axis, f32 angle) {
			m_rotation = Math::QuatFromAxisAngle(axis, angle) * m_rotation;
			UpdateTransform();
		}

		void Scale(const Vec3& scale) {
			m_scale *= scale;
			UpdateTransform();
		}

		void SetPosition(const Vec3& position) {
			m_position = position;
			UpdateTransform();
		}

		void SetRotation(const Vec3& euler) {
			m_rotation = Math::EulerToQuaternion(euler);
			UpdateTransform();
		}

		void SetRotation(const Quat& rotation) {
			m_rotation = rotation;
			UpdateTransform();
		}

		void SetScale(const Vec3& scale) {
			m_scale = scale;
			UpdateTransform();
		}

		Vec3        GetPosition() const { return m_position; }
		Quat        GetRotation() const { return m_rotation; }
		Vec3        GetScale() const { return m_scale; }
		Vec3        GetEulerRotation() const { return Math::QuaternionToEuler(m_rotation); }
		const Mat4& GetTransform() const { return m_transform; }

	  private:
		void UpdateTransform() { m_transform = Math::Translate(Mat4(1.0f), m_position) * Math::Mat4Cast(m_rotation) * Math::Scale(Mat4(1.0f), m_scale); }

		Vec3 m_position;
		Quat m_rotation;
		Vec3 m_scale;
		Mat4 m_transform;
	};
} // namespace Spark

#endif