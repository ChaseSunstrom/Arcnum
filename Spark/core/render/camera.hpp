#ifndef SPARK_CAMERA_HPP
#define SPARK_CAMERA_HPP

#include <core/math/math.hpp>
#include <core/pch.hpp>
#include <core/resource/asset.hpp>
#include <core/scene/transform.hpp>

namespace Spark {
	class Camera : public Asset<Camera> {
	  public:
		Camera(const String& name,
		       const Vec3&   position     = Vec3(0.0f, 0.0f, 5.0f),
		       const Vec3&   target       = Vec3(0.0f, 0.0f, 0.0f),
		       const Vec3&   up           = Vec3(0.0f, 1.0f, 0.0f),
		       f32           fov          = 45.0f,
		       f32           aspect_ratio = 16.0f / 9.0f,
		       f32           near         = 0.1f,
		       f32           far          = 100.0f)
			: Asset(name)
			, m_fov(fov)
			, m_aspect_ratio(aspect_ratio)
			, m_near(near)
			, m_far(far) {
			m_transform.SetPosition(position);

			m_transform.SetRotation(Quat(1, 0, 0, 0));

			if (position != target) {
				LookAt(target);
			}

			UpdateFrustum();
		}

		virtual ~Camera() = default;

		void             SetTransform(const Transform& transform) { m_transform = transform; }
		void             SetPosition(const Vec3& position) { m_transform.SetPosition(position); }
		void             SetPosition(f32 x, f32 y, f32 z) { m_transform.SetPosition(Vec3(x, y, z)); }
		const Vec3&      GetPosition() const { return m_transform.GetPosition(); }
		void             SetRotation(const Vec3& euler) { m_transform.SetRotation(Math::EulerToQuaternion(euler)); }
		void             SetRotation(const Quat& rotation) { m_transform.SetRotation(rotation); }
		Vec3             GetRotation() const { return Math::QuaternionToEuler(m_transform.GetRotation()); }
		void             SetScale(const Vec3& scale) { m_transform.SetScale(scale); }
		const Vec3&      GetScale() const { return m_transform.GetScale(); }
		Transform&       GetTransform() { return m_transform; }
		const Transform& GetTransform() const { return m_transform; }
		Mat4             GetViewMatrix() const { return Math::Inverse(m_transform.GetTransform()); }
		Mat4             GetProjectionMatrix() const { return Math::Perspective(Math::Radians(m_fov), m_aspect_ratio, m_near, m_far); }
		void             SetFOV(f32 fov) { m_fov = fov; }
		f32              GetFOV() const { return m_fov; }
		void             SetAspectRatio(f32 aspect_ratio) { m_aspect_ratio = aspect_ratio; }
		f32              GetAspectRatio() const { return m_aspect_ratio; }
		void             SetNear(f32 near) { m_near = near; }
		f32              GetNear() const { return m_near; }
		void             SetFar(f32 far) { m_far = far; }
		f32              GetFar() const { return m_far; }
		void             ZoomIn(f32 amount) { m_fov -= amount; }
		void             ZoomOut(f32 amount) { m_fov += amount; }
		void             Move(const Vec3& direction) { m_transform.Move(direction); }
		void             MoveX(f32 amount) { m_transform.MoveX(amount); }
		void             MoveY(f32 amount) { m_transform.MoveY(amount); }
		void             MoveZ(f32 amount) { m_transform.MoveZ(amount); }
		void             Rotate(const Vec3& euler) { m_transform.Rotate(euler); }
		void             RotateX(f32 angle) { m_transform.RotateX(angle); }
		void             RotateY(f32 angle) { m_transform.RotateY(angle); }
		void             RotateZ(f32 angle) { m_transform.RotateZ(angle); }
		void             Rotate(const Vec3& axis, f32 angle) { m_transform.Rotate(axis, angle); }
		void             Scale(const Vec3& scale) { m_transform.Scale(scale); }

		void LookAt(const Vec3& target) {
			Vec3 direction = target - m_transform.GetPosition();
			m_transform.SetRotation(Math::LookAt(direction));
		}

		void UpdateFrustum() {
			Mat4 view_projection = GetProjectionMatrix() * GetViewMatrix();
			m_frustum.ExtractFromViewProjection(view_projection);
		}

		bool IsPointVisible(const Vec3& point) const { return m_frustum.PointInFrustum(point); }
		bool IsSphereVisible(const Vec3& center, f32 radius) const { return m_frustum.SphereInFrustum(center, radius); }

	  private:
		Transform          m_transform;
		Math::Frustum<f32> m_frustum;
		f32                m_fov;
		f32                m_aspect_ratio;
		f32                m_near;
		f32                m_far;
	};
} // namespace Spark

#endif