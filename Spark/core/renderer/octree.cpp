#include "octree.hpp"

namespace spark
{
	void octree::ensure_contains(const math::vec3& point)
	{
		if (!is_inside(point))
		{
			// Calculate the new bounds required to include the point
			math::vec3 direction = normalize(point - m_center);
			while (!is_inside(point))
			{
				// Expand the root node size
				m_size *= 2;
				// Adjust the center based on the direction to the point
				m_center += direction * (m_size / 4);
			}
			// If the root node has children, re-insert objects to maintain the octree integrity
			if (!m_is_leaf)
			{
				std::vector<transform_component*> objects(std::move(m_transforms));
				for (auto* obj : objects)
				{
					insert(obj);
				}
			}
		}

		if (m_is_leaf && m_transforms.size() >= m_max_components)
		{
			subdivide();
		}
	}

	void octree::grow(const math::vec3& point)
	{
		math::vec3 direction = point - m_center;
		direction = normalize(direction);

		while (!is_inside(point))
		{
			m_size *= 2;
			m_center += direction * (m_size / 4);
		}

		if (!m_is_leaf)
		{
			for (auto* transform : m_transforms)
			{
				int32_t child_index = determine_child(transform->m_transform[3]);
				if (!m_children[child_index])
				{
					m_children[child_index] = std::make_unique<octree>(calculate_child_center(child_index), m_size / 2);
				}
				m_children[child_index]->insert(transform);
			}
			m_transforms.clear();
		}
	}

	math::vec3 octree::calculate_child_center(int32_t child_index) const
	{
		float32_t quarter_size = m_size / 4.0f;
		math::vec3 quarter_vec(
			(child_index & 1 ? quarter_size : -quarter_size),
			(child_index & 2 ? quarter_size : -quarter_size),
			(child_index & 4 ? quarter_size : -quarter_size));
		return m_center + quarter_vec;
	}

	void octree::subdivide()
	{
		if (!m_is_leaf || m_depth >= m_max_depth) // Check current depth before subdividing.
			return;

		m_is_leaf = false;

		float32_t half_size = m_size / 2.0f;

		for (int32_t i = 0; i < 8; ++i)
		{
			math::vec3 child_center = calculate_child_center(i);
			// Pass depth + 1 to the newly created children.
			m_children[i] = std::make_unique<octree>(child_center, half_size, m_depth + 1);
		}

		redistribute(); // Move existing transforms to the appropriate child nodes.
	}


	void octree::redistribute()
	{
		std::vector<transform_component*> tmp_transforms = std::move(m_transforms);
		for (auto* transform : tmp_transforms)
		{
			math::vec3 position = math::vec3(transform->m_transform[3]);
			int32_t child_index = determine_child(position);
			m_children[child_index]->insert(transform);
		}
	}

	void octree::insert(transform_component* transform)
	{
		math::vec3 position = math::vec3(transform->m_transform[3]);

		if (!is_inside(position))
			ensure_contains(position);

		if (m_is_leaf)
		{
			if (m_transforms.size() < m_max_components)
				m_transforms.emplace_back(transform);
			
			else
			{
				subdivide();
				if (!m_is_leaf)
				{
					int32_t child_index = determine_child(position);
					if (m_children[child_index])
						m_children[child_index]->insert(transform);
					else
						m_transforms.emplace_back(transform);
				}
				else
					m_transforms.emplace_back(transform);
			}
		}
		else
		{
			int32_t child_index = determine_child(position);
			m_children[child_index]->insert(transform);
		}
	}



	int32_t octree::determine_child(const math::vec3& position) const
	{
		int32_t child_index = 0;
		if (position.x >= m_center.x) child_index |= 1;
		if (position.y >= m_center.y) child_index |= 2;
		if (position.z >= m_center.z) child_index |= 4;
		return child_index;
	}

	int32_t octree::determine_child(const math::vec3& point, const math::vec3& center) const
	{
		int32_t child_index = 0;
		if (point.x >= center.x) child_index |= 1;
		if (point.y >= center.y) child_index |= 2;
		if (point.z >= center.z) child_index |= 4;
		return child_index;
	}

	bool octree::is_inside(const math::vec3& point) const
	{
		bool inside = true;
		inside &= point.x >= (m_center.x - m_size) && point.x <= (m_center.x + m_size);
		inside &= point.y >= (m_center.y - m_size) && point.y <= (m_center.y + m_size);
		inside &= point.z >= (m_center.z - m_size) && point.z <= (m_center.z + m_size);
		return inside;
	}

	bool octree::is_inside(const math::vec3& center, float32_t size) const
	{
		bool inside = true;
		// Check if the volume's min corner is inside
		inside &= (center.x - size) >= (m_center.x - m_size) && (center.x + size) <= (m_center.x + m_size);
		inside &= (center.y - size) >= (m_center.y - m_size) && (center.y + size) <= (m_center.y + m_size);
		inside &= (center.z - size) >= (m_center.z - m_size) && (center.z + size) <= (m_center.z + m_size);
		return inside;
	}

	bool octree::is_inside(const math::vec3& point, const math::vec3& min, const math::vec3& max) const
	{
		return (point.x >= min.x && point.x <= max.x) &&
			(point.y >= min.y && point.y <= max.y) &&
			(point.z >= min.z && point.z <= max.z);
	}

	bool octree::intersects(const frustum& frustum) const
	{
		for (const auto& plane : frustum.m_planes)
		{
			glm::vec3 closestPoint;
			closestPoint.x = plane.m_normal.x > 0 ? m_center.x - m_size / 2 : m_center.x + m_size / 2;
			closestPoint.y = plane.m_normal.y > 0 ? m_center.y - m_size / 2 : m_center.y + m_size / 2;
			closestPoint.z = plane.m_normal.z > 0 ? m_center.z - m_size / 2 : m_center.z + m_size / 2;

			if (plane.get_signed_distance(closestPoint) > 0)
			{
				return false; // AABB is completely outside the frustum
			}
		}
		return true; // Intersects or is completely inside the frustum
	}

	void octree::get_node_edges(std::vector<glm::vec3>& lines) const
	{
		glm::vec3 min_corner = m_center - glm::vec3(m_size / 2.0f);
		glm::vec3 max_corner = m_center + glm::vec3(m_size / 2.0f);

		// 8 corners of the AABB
		glm::vec3 corners[8] = {
			glm::vec3(min_corner.x, min_corner.y, min_corner.z),
			glm::vec3(max_corner.x, min_corner.y, min_corner.z),
			glm::vec3(max_corner.x, max_corner.y, min_corner.z),
			glm::vec3(min_corner.x, max_corner.y, min_corner.z),
			glm::vec3(min_corner.x, min_corner.y, max_corner.z),
			glm::vec3(max_corner.x, min_corner.y, max_corner.z),
			glm::vec3(max_corner.x, max_corner.y, max_corner.z),
			glm::vec3(min_corner.x, max_corner.y, max_corner.z)
		};

		// 12 edges of the AABB, each edge consists of 2 points
		int32_t edges[12][2] = {
			{0, 1}, {1, 2}, {2, 3}, {3, 0},
			{4, 5}, {5, 6}, {6, 7}, {7, 4},
			{0, 4}, {1, 5}, {2, 6}, {3, 7}
		};

		// Generate line segments for each edge
		for (int32_t i = 0; i < 12; ++i)
		{
			lines.push_back(corners[edges[i][0]]);
			lines.push_back(corners[edges[i][1]]);
		}
	}


	void octree::collect_visible(const frustum& frustum, std::vector<math::mat4>& transforms) const
	{
		// First, check if the current node intersects the frustum.
		if (!this->intersects(frustum)) return;

		// If it intersects, add all objects in the current node.
		for (auto* transform : m_transforms)
		{
			transforms.emplace_back(transform->m_transform);
		}

		// Recursively check child nodes if this is not a leaf.
		if (!m_is_leaf)
		{
			for (const auto& child : m_children)
			{
				if (child)
				{ // Ensure child exists before trying to access it
					child->collect_visible(frustum, transforms);
				}
			}
		}
	}
}