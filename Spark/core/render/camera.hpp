#ifndef SPARK_CAMERA_HPP
#define SPARK_CAMERA_HPP

#include <core/math/math.hpp>
#include <core/pch.hpp>
#include <core/resource/asset.hpp>
#include <core/scene/transform.hpp>

namespace Spark {
	struct Frustum {
		_MATH Vec4 planes[6]; // Left, Right, Bottom, Top, Near, Far

		void Update(const _MATH Mat4& view_projection) {
			// Extract frustum planes from view-projection matrix
			for (i32 i = 0; i < 6; ++i) {
				i32 row   = i / 2;
				planes[i] = (i % 2 == 0) ? view_projection[3] + view_projection[row] : view_projection[3] - view_projection[row];
				planes[i] = _MATH Normalize(planes[i]);
			}
		}

		bool IsPointInside(const _MATH Vec3& point) const {
			for (i32 i = 0; i < 6; ++i) {
				if (_MATH Dot(_MATH Vec3(planes[i]), point) + planes[i].w < 0) {
					return false;
				}
			}
			return true;
		}

		bool IsSphereInside(const _MATH Vec3& center, f32 radius) const {
			for (i32 i = 0; i < 6; ++i) {
				if (_MATH Dot(_MATH Vec3(planes[i]), center) + planes[i].w < -radius) {
					return false;
				}
			}
			return true;
		}
	};

	class Camera : public Asset<Camera> {
	  public:
		Camera(const String&     name,
		       const _MATH Vec3& position                = _MATH Vec3(0.0f, 0.0f, 5.0f),
		       const _MATH Vec3& target                  = _MATH Vec3(0.0f, 0.0f, 0.0f),
		       const _MATH Vec3& up                      = _MATH Vec3(0.0f, 1.0f, 0.0f),
		       f32                          fov          = 45.0f,
		       f32                          aspect_ratio = 16.0f / 9.0f,
		       f32                          near         = 0.1f,
		       f32                          far          = 100.0f)
			: Asset(name)
			, m_fov(fov)
			, m_aspect_ratio(aspect_ratio)
			, m_near(near)
			, m_far(far) {
			// Set the position
			m_transform.SetPosition(position);

			// Set rotation to identity (looking down the negative z-axis)
			m_transform.SetRotation(_MATH Quat(1, 0, 0, 0));

			// If you want the camera to look at the target:
			if (position != target) {
				LookAt(target);
			}

			UpdateFrustum();
		}
		virtual ~Camera() = default;
		void             SetTransform(const Transform& transform) { m_transform = transform; }
		void             SetPosition(const _MATH Vec3& position) { m_transform.SetPosition(position); }
		void             SetPosition(f32 x, f32 y, f32 z) { m_transform.SetPosition(_MATH Vec3(x, y, z)); }
		const _MATH Vec3 GetPosition() const { return m_transform.GetPosition(); }
		void             SetRotation(const _MATH Vec3& euler) { m_transform.SetRotation(_MATH EulerToQuaternion(euler)); }
		void             SetRotation(const _MATH Quat& rotation) { m_transform.SetRotation(rotation); }
		const _MATH Vec3 GetRotation() const { return _MATH QuaternionToEuler(m_transform.GetRotation()); }
		void             SetScale(const _MATH Vec3& scale) { m_transform.SetScale(scale); }
		const _MATH Vec3 GetScale() const { return m_transform.GetScale(); }
		Transform&       GetTransform() { return m_transform; }
		const Transform& GetTransform() const { return m_transform; }
		_MATH Mat4       GetViewMatrix() const { return _MATH Inverse(m_transform.GetTransform()); }
		_MATH Mat4       GetProjectionMatrix() const { return _MATH Perspective(_MATH Radians(m_fov), m_aspect_ratio, m_near, m_far); }
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
		void             Move(const _MATH Vec3& direction) { m_transform.Move(direction); }
		void             MoveX(f32 amount) { m_transform.MoveX(amount); }
		void             MoveY(f32 amount) { m_transform.MoveY(amount); }
		void             MoveZ(f32 amount) { m_transform.MoveZ(amount); }
		void             Rotate(const _MATH Vec3& euler) { m_transform.Rotate(euler); }
		void             RotateX(f32 angle) { m_transform.RotateX(angle); }
		void             RotateY(f32 angle) { m_transform.RotateY(angle); }
		void             RotateZ(f32 angle) { m_transform.RotateZ(angle); }
		void             Rotate(const _MATH Vec3& axis, f32 angle) { m_transform.Rotate(axis, angle); }
		void             Scale(const _MATH Vec3& scale) { m_transform.Scale(scale); }
		void             LookAt(const _MATH Vec3& target) { m_transform.SetRotation(_MATH EulerAngles(_MATH Inverse(_MATH LookAt(m_transform.GetPosition(), target, _MATH Vec3(0, 1, 0))))); }

		void UpdateFrustum() {
			_MATH Mat4 view_projection = GetProjectionMatrix() * GetViewMatrix();
			m_frustum.Update(view_projection);
		}

		bool IsPointVisible(const _MATH Vec3& point) const { return m_frustum.IsPointInside(point); }
		bool IsSphereVisible(const _MATH Vec3& center, f32 radius) const { return m_frustum.IsSphereInside(center, radius); }

	  private:
		Transform m_transform;
		Frustum   m_frustum;
		f32       m_fov;
		f32       m_aspect_ratio;
		f32       m_near;
		f32       m_far;
	};
} // namespace Spark

#endif