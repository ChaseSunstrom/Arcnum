#ifndef SPARK_CAMERA_HPP
#define SPARK_CAMERA_HPP

#include "spark_pch.hpp"
#include "spark_math.hpp"
#include "spark_transform.hpp"

namespace spark {

    enum class ProjectionMode
    {
        PERSPECTIVE,
        ORTHOGRAPHIC
    };

    class Camera
    {
    public:
        // - 'position' is the camera position.
        // - 'target' is the point the camera looks at.
        // - 'fov' (in degrees), 'aspect_ratio', 'near_plane' and 'far_plane' define the perspective projection.
        // - 'projection_mode' selects between perspective and orthographic.
        Camera(const math::Vec3& position = math::Vec3(0.0f, 0.0f, 0.0f),
            const math::Vec3& target = math::Vec3(0.0f, 0.0f, -1.0f),
            f32 fov = 45.0f,
            f32 aspect_ratio = 16.0f / 9.0f,
            f32 near_plane = 0.1f,
            f32 far_plane = 100.0f,
            ProjectionMode projection_mode = ProjectionMode::PERSPECTIVE)
            : m_transform(position)
            , m_fov(fov)
            , m_aspect_ratio(aspect_ratio)
            , m_near_plane(near_plane)
            , m_far_plane(far_plane)
            , m_projection_mode(projection_mode)
            , m_ortho_scale(1.0f)
        {
            LookAt(target);
            UpdateProjection();
            UpdateFrustum();
        }

        void SetFOV(f32 fov)
        {
            m_fov = fov;
            UpdateProjection();
            UpdateFrustum();
        }

        f32 GetFOV() const
        {
            return m_fov;
        }

        void SetAspectRatio(f32 aspect_ratio)
        {
            m_aspect_ratio = aspect_ratio;
            UpdateProjection();
            UpdateFrustum();
        }

        f32 GetAspectRatio() const
        {
            return m_aspect_ratio;
        }

        void SetNearPlane(f32 near_plane)
        {
            m_near_plane = near_plane;
            UpdateProjection();
            UpdateFrustum();
        }

        f32 GetNearPlane() const
        {
            return m_near_plane;
        }

        void SetFarPlane(f32 far_plane)
        {
            m_far_plane = far_plane;
            UpdateProjection();
            UpdateFrustum();
        }

        f32 GetFarPlane() const
        {
            return m_far_plane;
        }

        void SetProjectionMode(ProjectionMode mode)
        {
            m_projection_mode = mode;
            UpdateProjection();
            UpdateFrustum();
        }

        ProjectionMode GetProjectionMode() const
        {
            return m_projection_mode;
        }


        void SetPosition(const math::Vec3& position)
        {
            m_transform.SetPosition(position);
            UpdateFrustum();
        }

        math::Vec3 GetPosition() const
        {
            return m_transform.GetPosition();
        }

        void Move(const math::Vec3& delta)
        {
            m_transform.Move(delta);
            UpdateFrustum();
        }

        void LookAt(const math::Vec3& target)
        {
            math::Vec3 eye = GetPosition();
            math::Vec3 up(0.0f, 1.0f, 0.0f);
            math::Mat4 view = math::CreateViewMatrix(eye, target, up);
            // Since view = Inverse( transform ), we compute the camera rotation from the inverse
            math::Mat4 camera_transform = math::Inverse(view);
            // Extract rotation from the transform matrix.
            math::Quat rotation = math::QuatCast(camera_transform);
            m_transform.SetRotation(rotation);
            UpdateFrustum();
        }

        math::Mat4 GetViewMatrix() const
        {
            return math::Inverse(m_transform.GetTransform());
        }

        math::Mat4 GetProjectionMatrix() const
        {
            return m_projection;
        }

        math::Mat4 GetViewProjectionMatrix() const
        {
            return GetViewMatrix() * m_projection;
        }

        // --- Frustum access (for culling) ---
        const math::Frustum<f32>& GetFrustum() const
        {
            return m_frustum;
        }

        // --- Extra camera controls ---

        // Orbit: rotates the camera around its current target.
        // 'yaw' and 'pitch' are given in radians.
        void Orbit(f32 yaw, f32 pitch)
        {
            // For an orbit we assume the camera maintains a distance from a target.
            // Here we update the camera rotation by pre‐multiplying small yaw and pitch rotations.
            math::Quat q_yaw = math::QuatFromAxisAngle(math::Vec3(0.0f, 1.0f, 0.0f), yaw);
            math::Quat q_pitch = math::QuatFromAxisAngle(math::Vec3(1.0f, 0.0f, 0.0f), pitch);
            math::Quat new_rotation = q_pitch * m_transform.GetRotation() * q_yaw;
            m_transform.SetRotation(new_rotation);
            UpdateFrustum();
        }

        // Pan: translates the camera parallel to the view plane.
        void Pan(const math::Vec3& delta)
        {
            // Compute right and up vectors from the camera transform.
            math::Vec3 right = GetRight();
            math::Vec3 up = GetUp();
            math::Vec3 translation = right * delta.x + up * delta.y;
            m_transform.Move(translation);
            UpdateFrustum();
        }

        // Zoom: for perspective, adjusts the FOV; for orthographic, adjusts the ortho scale.
        void Zoom(f32 amount)
        {
            if (m_projection_mode == ProjectionMode::PERSPECTIVE)
            {
                m_fov = math::Clamp(m_fov - amount, 1.0f, 120.0f);
            }
            else
            {
                m_ortho_scale = math::Max(m_ortho_scale - amount, 0.1f);
            }
            UpdateProjection();
            UpdateFrustum();
        }

        // --- Direction vectors ---

        // Returns the camera's forward vector (the direction it is looking at).
        math::Vec3 GetForward() const
        {
            // In our transform the local forward is -Z.
            math::Mat4 transform = m_transform.GetTransform();
            math::Vec3 forward(transform(0, 2), transform(1, 2), transform(2, 2));
            return -math::Normalize(forward);
        }

        // Returns the right vector (local +X).
        math::Vec3 GetRight() const
        {
            math::Mat4 transform = m_transform.GetTransform();
            math::Vec3 right(transform(0, 0), transform(1, 0), transform(2, 0));
            return math::Normalize(right);
        }

        // Returns the up vector (local +Y).
        math::Vec3 GetUp() const
        {
            math::Mat4 transform = m_transform.GetTransform();
            math::Vec3 up(transform(0, 1), transform(1, 1), transform(2, 1));
            return math::Normalize(up);
        }

    private:
        // Updates the projection matrix based on the current projection mode and parameters.
        void UpdateProjection()
        {
            if (m_projection_mode == ProjectionMode::PERSPECTIVE)
            {
                m_projection = math::Perspective(math::Radians(m_fov), m_aspect_ratio, m_near_plane, m_far_plane);
            }
            else
            {
                // For orthographic projection we choose left/right/bottom/top based on an ortho scale.
                f32 half_width = m_ortho_scale * m_aspect_ratio;
                f32 half_height = m_ortho_scale;
                f32 left = -half_width;
                f32 right = half_width;
                f32 bottom = -half_height;
                f32 top = half_height;
                m_projection = math::CreateOrthographicRH(left, right, bottom, top, m_near_plane, m_far_plane);
            }
        }

        // Updates the frustum based on the current view-projection matrix.
        void UpdateFrustum()
        {
            math::Mat4 view_projection = GetViewProjectionMatrix();
            m_frustum.ExtractFromViewProjection(view_projection);
        }

        Transform          m_transform;
        f32                m_fov;
        f32                m_aspect_ratio;
        f32                m_near_plane;
        f32                m_far_plane;
        ProjectionMode    m_projection_mode;
        // For orthographic projection: scale controls the size of the view volume.
        f32                m_ortho_scale;
        // Cached projection matrix
        math::Mat4         m_projection;
        // Frustum for culling
        math::Frustum<f32> m_frustum;
    };

} // namespace spark

#endif // SPARK_CAMERA_HPP
