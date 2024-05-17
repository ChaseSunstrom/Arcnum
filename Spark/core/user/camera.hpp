#ifndef SPARK_CAMERA_HPP
#define SPARK_CAMERA_HPP

#include "../renderer/frustum.hpp"
#include "../spark.hpp"
#include "../util/singleton.hpp"

namespace Spark
{
struct Camera
{
    Camera(const Math::vec3 &position = Math::vec3(0.0f, 0.0f, 3.0f),
           const Math::vec3 &direction = Math::vec3(0.0f, 0.0f, -1.0f), f32 fov = 90.0f, f32 aspect_ratio = 1,
           f32 near_plane = 0.1f, f32 far_plane = 100.0f)
        : m_position(position), m_direction(direction), m_fov(fov), m_aspect_ratio(aspect_ratio),
          m_near_plane(near_plane), m_far_plane(far_plane)
    {
        m_frustum = std::make_unique<Frustum>(get_view_projection_matrix());
    }

    Math::mat4 get_view_matrix() const
    {
        return Math::lookAt(m_position, m_position + m_direction, Math::vec3(0.0f, 1.0f, 0.0f));
    }

    Math::mat4 get_projection_matrix() const
    {
        return Math::perspective(Math::radians(m_fov), m_aspect_ratio, m_near_plane, m_far_plane);
    }

    Math::mat4 get_view_projection_matrix() const
    {
        return get_projection_matrix() * get_view_matrix();
    }

    bool operator!=(const Camera &other) const
    {
        return !(m_position == other.m_position && m_direction == other.m_direction && m_fov == other.m_fov &&
                 m_aspect_ratio == other.m_aspect_ratio && m_near_plane == other.m_near_plane &&
                 m_far_plane == other.m_far_plane);
    }

    Math::vec3 m_position;

    Math::vec3 m_direction;

    f32 m_fov;

    f32 m_aspect_ratio;

    f32 m_near_plane;

    f32 m_far_plane;

    std::unique_ptr<Frustum> m_frustum;
};

class CameraManager : public Singleton<CameraManager>
{
    friend class Singleton<CameraManager>;
  public:

    Camera& get_camera(std::string name)
    {
        return *m_cameras[name];
    }

    void add_camera(std::string name, std::unique_ptr<Camera> camera)
    {
        m_cameras[name] = std::move(camera);
    }

  private:
    CameraManager() = default;

    ~CameraManager() = default;
  private:
    std::unordered_map<std::string, std::unique_ptr<Camera>> m_cameras;
};
} // namespace Spark

#endif