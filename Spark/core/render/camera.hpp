#ifndef SPARK_CAMERA_HPP
#define SPARK_CAMERA_HPP

#include <core/math/math.hpp>
#include <core/pch.hpp>
#include <core/scene/transform.hpp>
#include <core/system/manager.hpp>
#include <core/event/event.hpp>

namespace Spark {
	struct Frustum {
		Math::Vec4 planes[6]; // Left, Right, Bottom, Top, Near, Far

		void Update(const Math::Mat4& view_projection) {
			// Extract frustum planes from view-projection matrix
			for (i32 i = 0; i < 6; ++i) {
				i32 row   = i / 2;
				planes[i] = (i % 2 == 0) ? view_projection[3] + view_projection[row] : view_projection[3] - view_projection[row];
				planes[i] = Math::Normalize(planes[i]);
			}
		}

		bool IsPointInside(const Math::Vec3& point) const {
			for (i32 i = 0; i < 6; ++i) { if (Math::Dot(Math::Vec3(planes[i]), point) + planes[i].w < 0) { return false; } }
			return true;
		}

		bool IsSphereInside(const Math::Vec3& center, f32 radius) const {
			for (i32 i = 0; i < 6; ++i) { if (Math::Dot(Math::Vec3(planes[i]), center) + planes[i].w < -radius) { return false; } }
			return true;
		}
	};

	class Camera {
	public:
		friend class Manager<Camera>;
		~Camera() = default;
		void             SetTransform(const Transform& transform) { m_transform = transform; }
		void             SetPosition(const Math::Vec3& position) { m_transform.SetPosition(position); }
		void             SetPosition(f32 x, f32 y, f32 z) { m_transform.SetPosition(Math::Vec3(x, y, z)); }
		const Math::Vec3 GetPosition() const { return m_transform.GetPosition(); }
		void             SetRotation(const Math::Vec3& euler) { m_transform.SetRotation(Math::EulerToQuaternion(euler)); }
		void             SetRotation(const Math::Quat& rotation) { m_transform.SetRotation(rotation); }
		const Math::Vec3 GetRotation() const { return Math::QuaternionToEuler(m_transform.GetRotation()); }
		void             SetScale(const Math::Vec3& scale) { m_transform.SetScale(scale); }
		const Math::Vec3 GetScale() const { return m_transform.GetScale(); }
		Transform&       GetTransform() { return m_transform; }
		const Transform& GetTransform() const { return m_transform; }
		Math::Mat4       GetViewMatrix() const { return Math::Inverse(m_transform.GetTransform()); }
		Math::Mat4       GetProjectionMatrix() const { return Math::Perspective(Math::Radians(m_fov), m_aspect_ratio, m_near, m_far); }
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
		void             Move(const Math::Vec3& direction) { m_transform.Move(direction); }
		void             MoveX(f32 amount) { m_transform.MoveX(amount); }
		void             MoveY(f32 amount) { m_transform.MoveY(amount); }
		void             MoveZ(f32 amount) { m_transform.MoveZ(amount); }
		void             Rotate(const Math::Vec3& euler) { m_transform.Rotate(euler); }
		void             RotateX(f32 angle) { m_transform.RotateX(angle); }
		void             RotateY(f32 angle) { m_transform.RotateY(angle); }
		void             RotateZ(f32 angle) { m_transform.RotateZ(angle); }
		void             Rotate(const Math::Vec3& axis, f32 angle) { m_transform.Rotate(axis, angle); }
		void             Scale(const Math::Vec3& scale) { m_transform.Scale(scale); }
		void             LookAt(const Math::Vec3& target) { m_transform.SetRotation(Math::EulerAngles(Math::Inverse(Math::LookAt(m_transform.GetPosition(), target, Math::Vec3(0, 1, 0))))); }

		void UpdateFrustum() {
			Math::Mat4 view_projection = GetProjectionMatrix() * GetViewMatrix();
			m_frustum.Update(view_projection);
		}

		bool IsPointVisible(const Math::Vec3& point) const { return m_frustum.IsPointInside(point); }
		bool IsSphereVisible(const Math::Vec3& center, f32 radius) const { return m_frustum.IsSphereInside(center, radius); }

	private:
		Camera(const Math::Vec3& position     = Math::Vec3(0.0f, 0.0f, 5.0f),
		       const Math::Vec3& target       = Math::Vec3(0.0f, 0.0f, 0.0f),
		       const Math::Vec3& up           = Math::Vec3(0.0f, 1.0f, 0.0f),
		       f32               fov          = 45.0f,
		       f32               aspect_ratio = 16.0f / 9.0f,
		       f32               near         = 0.1f,
		       f32               far          = 100.0f)
			: m_fov(fov)
			, m_aspect_ratio(aspect_ratio)
			, m_near(near)
			, m_far(far) {
			// Set the position
			m_transform.SetPosition(position);

			// Set rotation to identity (looking down the negative z-axis)
			m_transform.SetRotation(Math::Quat(1, 0, 0, 0));

			// If you want the camera to look at the target:
			if (position != target) { LookAt(target); }

			UpdateFrustum();
		}

	private:
		Transform m_transform;
		Frustum   m_frustum;
		f32       m_fov;
		f32       m_aspect_ratio;
		f32       m_near;
		f32       m_far;
	};

	template<> class Manager<Camera> : public IManager {
	public:
		Manager()
			: m_registry(std::make_unique<Registry<Camera>>()) {
			// Default camera
			Create("Default Camera");
		}

		~Manager() = default;

		template<typename... Args> RefPtr<Camera> Create(const std::string& name, Args&&... args) {
			Camera* object = new Camera(std::forward<Args>(args)...);
			Register(name, std::unique_ptr<Camera>(object));

			if (GetSize() == 1)
				m_current_camera = Get(name);

			return m_current_camera;
		}

		u32 GetSize() const { return m_registry->GetSize(); }

		void OnEvent(const EventPtr<WindowResizedEvent> event) { m_current_camera->SetAspectRatio(static_cast<f32>(event->width) / static_cast<f32>(event->height)); }

		RefPtr<Camera> Register(const std::string& name, std::unique_ptr<Camera> object) { return m_registry->Register(name, std::move(object)); }

		RefPtr<Camera> Get(const std::string& name) const { return m_registry->Get(name); }

		RefPtr<Camera> Get(const Handle handle) const { return m_registry->Get(handle); }

		RefPtr<Camera> GetCurrentCamera() const { return m_current_camera; }

		RefPtr<Camera> SetCurrentCamera(const std::string& name) {
			m_current_camera = m_registry->Get(name);
			return m_current_camera;
		}

	private:
		RefPtr<Camera>                    m_current_camera;
		std::unique_ptr<Registry<Camera>> m_registry;
	};
} // namespace Spark

#endif