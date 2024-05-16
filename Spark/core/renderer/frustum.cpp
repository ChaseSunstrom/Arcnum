#include "frustum.hpp"

namespace Spark
{
Frustum::Frustum(const Math::mat4 &pv_matrix)
{
    m_view_projection = pv_matrix;
    update(pv_matrix);
}

void Frustum::update(const Math::mat4 &pv_matrix)
{
    // Left Plane
    m_planes[0] = Plane(Math::vec3(pv_matrix[0][3] + pv_matrix[0][0], pv_matrix[1][3] + pv_matrix[1][0],
                                   pv_matrix[2][3] + pv_matrix[2][0]),
                        pv_matrix[3][3] + pv_matrix[3][0]);

    // Right Plane
    m_planes[1] = Plane(Math::vec3(pv_matrix[0][3] - pv_matrix[0][0], pv_matrix[1][3] - pv_matrix[1][0],
                                   pv_matrix[2][3] - pv_matrix[2][0]),
                        pv_matrix[3][3] - pv_matrix[3][0]);

    // Bottom Plane
    m_planes[2] = Plane(Math::vec3(pv_matrix[0][3] + pv_matrix[0][1], pv_matrix[1][3] + pv_matrix[1][1],
                                   pv_matrix[2][3] + pv_matrix[2][1]),
                        pv_matrix[3][3] + pv_matrix[3][1]);

    // Top Plane
    m_planes[3] = Plane(Math::vec3(pv_matrix[0][3] - pv_matrix[0][1], pv_matrix[1][3] - pv_matrix[1][1],
                                   pv_matrix[2][3] - pv_matrix[2][1]),
                        pv_matrix[3][3] - pv_matrix[3][1]);

    // Near Plane
    m_planes[4] = Plane(Math::vec3(pv_matrix[0][3] + pv_matrix[0][2], pv_matrix[1][3] + pv_matrix[1][2],
                                   pv_matrix[2][3] + pv_matrix[2][2]),
                        pv_matrix[3][3] + pv_matrix[3][2]);

    // Far Plane
    m_planes[5] = Plane(Math::vec3(pv_matrix[0][3] - pv_matrix[0][2], pv_matrix[1][3] - pv_matrix[1][2],
                                   pv_matrix[2][3] - pv_matrix[2][2]),
                        pv_matrix[3][3] - pv_matrix[3][2]);

    // Normalize all planes
    for (auto &plane : m_planes)
    {
        float length = Math::length(plane.m_normal);
        plane.m_normal /= length;
        plane.m_distance /= length;
    }
}

std::array<Math::vec3, 8> Frustum::get_corners() const
{
    // Define an array to hold the corners
    std::array<Math::vec3, 8> corners = std::array<Math::vec3, 8>();

    // The inverse of the view-projection matrix will take points from
    // clip space back to world space.
    Math::mat4 inv_vp = Math::inverse(m_view_projection);

    // Define the NDC (Normalized Device Coordinates) frustum corners
    std::array<Math::vec4, 8> ndc_corners = {
        Math::vec4(-1.0f, 1.0f, -1.0f, 1.0f),  // near top left
        Math::vec4(1.0f, 1.0f, -1.0f, 1.0f),   // near top right
        Math::vec4(-1.0f, -1.0f, -1.0f, 1.0f), // near bottom left
        Math::vec4(1.0f, -1.0f, -1.0f, 1.0f),  // near bottom right
        Math::vec4(-1.0f, 1.0f, 1.0f, 1.0f),   // far top left
        Math::vec4(1.0f, 1.0f, 1.0f, 1.0f),    // far top right
        Math::vec4(-1.0f, -1.0f, 1.0f, 1.0f),  // far bottom left
        Math::vec4(1.0f, -1.0f, 1.0f, 1.0f)    // far bottom right
    };

    // Transform the NDC corners back to world space
    for (int i = 0; i < ndc_corners.size(); ++i)
    {
        // Transform the NDC corners to world space
        Math::vec4 world_space_corner = inv_vp * ndc_corners[i];
        // Perform perspective divide to get the 3D world space coordinates
        corners[i] = Math::vec3(world_space_corner) / world_space_corner.w;
    }

    return corners;
}

bool Frustum::is_inside(const Math::vec3 &center, float radius) const
{
    for (const auto &plane : m_planes)
    {
        if (plane.get_signed_distance(center) < -radius)
        {
            return false; // Sphere center - radius is outside the plane, hence
                          // outside the frustum
        }
    }

    return true; // Sphere is either fully inside the frustum or intersects it
}
} // namespace Spark