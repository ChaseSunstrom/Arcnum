#include "octree.hpp"

namespace spark
{
	void octree::ensure_contains(const math::vec3& point)
	{
		if (!is_inside(point))
		{
			grow(point); // Grow the octree to include the point
		}

		if (m_is_leaf && m_transforms.size() >= m_max_components)
		{
			subdivide(); // Subdivide if this node reaches the maximum capacity
		}
	}

	void octree::redistribute_if_necessary()
	{
		// Only redistribute if this node is the root node
		if (m_parent != nullptr)
			return;

		// Redistribute objects to children if the root node has objects and has grown
		if (m_transforms.size() > 0)
		{
			subdivide(); // This will create children nodes.
		}
	}


	math::vec3 octree::calculate_nearest_octant_center(const math::vec3& point) const
	{
		// Direction vectors to choose the nearest octant
		math::vec3 direction(
			(point.x < m_center.x) ? -1.0f : 1.0f,
			(point.y < m_center.y) ? -1.0f : 1.0f,
			(point.z < m_center.z) ? -1.0f : 1.0f);

		// Calculate new center to include the point. 
		// We are moving the center by half the width of the new, larger octree.
		math::vec3 new_center = m_center + direction * (m_size / 2.0f);

		// The point should be inside the new octree bounds, considering its full size, not just half.
		// We check against the full size since the point can be anywhere in the octant.
		bool is_inside_new_bounds =
			std::abs(point.x - new_center.x) <= (m_size / 2.0f) &&
			std::abs(point.y - new_center.y) <= (m_size / 2.0f) &&
			std::abs(point.z - new_center.z) <= (m_size / 2.0f);

		assert(is_inside_new_bounds);

		return new_center;
	}

	math::vec3 octree::calculate_min_point() const
	{
		math::vec3 min_point = m_center;
		min_point -= m_size / 2.0f;
		return min_point;
	}

	math::vec3 octree::calculate_max_point() const
	{
		math::vec3 max_point = m_center;
		max_point += m_size / 2.0f;
		return max_point;
	}

	void octree::grow(const math::vec3& position)
	{
		// Only update the root node's size and center
		if (m_parent != nullptr)
			return;

		// Find the minimum and maximum points of all existing nodes
		math::vec3 min_point = m_center - m_size / 2.0f;
		math::vec3 max_point = m_center + m_size / 2.0f;

		// Recursively find the minimum and maximum points considering all nodes
		find_min_max_points(min_point, max_point);

		// Calculate the new size and center to encompass all nodes
		math::vec3 new_center = (min_point + max_point) / 2.0f;
		float new_size = math::distance(min_point, max_point);

		// Update the root node's size and center
		m_size = new_size;
		m_center = new_center;
	}

	void octree::find_min_max_points(math::vec3& min_point, math::vec3& max_point) const
	{
		// Iterate over all nodes to find the minimum and maximum points
		min_point = math::min(min_point, m_center - m_size / 2.0f);
		max_point = math::max(max_point, m_center + m_size / 2.0f);

		// Recursively find min and max points in child nodes
		if (!m_is_leaf)
		{
			for (const auto& child : m_children)
			{
				if (child)
				{
					child->find_min_max_points(min_point, max_point);
				}
			}
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
		if (!m_is_leaf || m_depth >= m_max_depth)
			return;

		// Update leaf status
		m_is_leaf = false;

		// Calculate child centers
		float32_t quarter_size = m_size / 4.0f;
		math::vec3 child_centers[8];
		for (int32_t i = 0; i < 8; ++i)
		{
			child_centers[i] = calculate_child_center(i);
		}

		// Create children nodes
		for (int32_t i = 0; i < 8; ++i)
		{
			m_children[i] = std::make_unique<octree>(child_centers[i], m_size / 2.0f, m_depth + 1, this);
		}

		// Redistribute objects to children
		for (auto it = m_transforms.begin(); it != m_transforms.end();)
		{
			int32_t child_index = determine_child((*it)->m_transform[3]);
			if (m_children[child_index])
			{
				m_children[child_index]->insert(*it);
				it = m_transforms.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void octree::redistribute_to_parent()
	{
		// Check if this node is the root node (i.e., has no parent).
		if (m_parent == nullptr)
		{
			// The current node is the root; redistribute its objects to its children.
			redistribute();
		}
		else
		{
			// The current node is not the root; move the objects up to the parent.
			std::vector<transform_component*> tmp_transforms = std::move(m_transforms);
			for (auto* transform : tmp_transforms)
			{
				// Insert the objects into the parent node, which will place them into the correct child.
				m_parent->insert(transform);
			}
			// Now that the objects have been moved to the parent, clear the current node's objects.
			m_transforms.clear();
		}
	}

	void octree::redistribute()
	{
		if (m_is_leaf)
		{
			// Do not redistribute if this is a leaf, there are no children to redistribute to.
			return;
		}

		// Redistribute objects to children
		for (auto it = m_transforms.begin(); it != m_transforms.end();)
		{
			int32_t child_index = determine_child((*it)->m_transform[3]);
			if (m_children[child_index])
			{
				m_children[child_index]->insert(*it);
				it = m_transforms.erase(it);
			}
			else
			{
				++it;
			}
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