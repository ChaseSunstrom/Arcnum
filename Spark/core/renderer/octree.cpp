#include "octree.hpp"

namespace spark
{
	void octree::ensure_contains(const math::vec3& point)
	{
		// Grow the octree if the point is not inside.
		if (!is_inside(point))
		{
			grow(point);
		}

		// Perform redistribution if necessary after growing.
		redistribute_if_necessary();

		// Check whether this node should subdivide.
		if (m_is_leaf && m_transforms.size() >= m_max_components)
		{
			subdivide(); // Only subdivide if this is a leaf and has reached max capacity.
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

	bool octree::is_inside(const math::vec3& point, const math::vec3& center, float half_size) const
	{
		// Check if the point is inside the cube defined by the new center and half the size.
		return (point.x >= (center.x - half_size) && point.x <= (center.x + half_size)) &&
			(point.y >= (center.y - half_size) && point.y <= (center.y + half_size)) &&
			(point.z >= (center.z - half_size) && point.z <= (center.z + half_size));
	}

	void octree::grow(const math::vec3& point)
	{
		bool grew = false;
		while (!is_inside(point, m_center, m_size / 2.0f))
		{
			// Adjust the octree size to ensure it contains the point
			m_size *= 2.0f;
			math::vec3 direction(
				point.x < m_center.x ? -1.0f : 1.0f,
				point.y < m_center.y ? -1.0f : 1.0f,
				point.z < m_center.z ? -1.0f : 1.0f);
			m_center += direction * (m_size / 4.0f); // Move center towards the point

			grew = true;
		}

		if (grew)
		{
			if (m_is_leaf)
			{
				subdivide();
			}
			else
			{
				// Adjust children for the new size and reposition them accordingly
				adjust_children();
				// Now redistribute transforms in all nodes to ensure even distribution
				redistribute();
			}
		}
	}

	void octree::adjust_children()
	{
		if (m_is_leaf) return; // No children to adjust

		// Recreate children with updated size and position
		for (int i = 0; i < 8; ++i)
		{
			m_children[i] = std::make_unique<octree>(
				calculate_child_center(i), m_size / 2.0f, m_depth + 1, this);
		}
	}

	void octree::merge(std::unique_ptr<octree>&& old_child)
	{
		// Check if the old child node is a leaf and transfer objects
		if (old_child->m_is_leaf)
		{
			for (auto& transform : old_child->m_transforms)
			{
				// Directly insert transforms into the new child node
				insert(transform);
			}
		}
		else
		{
			// If the old child has its own children, they need to be redistributed
			// to fit within the new structure. This can be done by recursively
			// inserting the objects into the current octree.
			for (auto& old_grandchild : old_child->m_children)
			{
				if (old_grandchild)
				{
					// Recursively merge each grandchild.
					merge(std::move(old_grandchild));
				}
			}
		}

		// After transferring the objects, the old child's node's children, if any,
		// should be distributed among the new child node's children. Since the
		// `merge` method is only called when the octree grows, the old child node
		// can be discarded after its data is transferred.
	}

	void octree::reposition_children()
	{
		// For each child, calculate its new position based on the new size of the octree.
		for (int i = 0; i < 8; ++i)
		{
			if (m_children[i])
			{
				// Calculate the new center for this child.
				m_children[i]->m_center = calculate_child_center(i);

				// Recursively reposition the children of this child.
				m_children[i]->reposition_children();
			}
		}
	}


	bool octree::is_inside(const math::vec3& point, float size, const math::vec3& center, float half_size) const
	{
		// Check if the point is inside the cube defined by center and size.
		return (point.x >= (center.x - half_size) && point.x <= (center.x + half_size) &&
				point.y >= (center.y - half_size) && point.y <= (center.y + half_size) &&
				point.z >= (center.z - half_size) && point.z <= (center.z + half_size));
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
		if (m_depth >= m_max_depth || m_is_leaf == false)
			return;

		m_is_leaf = false;
		for (int i = 0; i < 8; ++i)
		{
			m_children[i] = std::make_unique<octree>(
				calculate_child_center(i), m_size / 2.0f, m_depth + 1, this);
		}

		// Redistribute existing transforms to the new children
		redistribute_to_children();
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
	
	void octree::redistribute_to_children()
	{
		if (m_is_leaf) return; // Cannot redistribute from a leaf

		std::vector<transform_component*> tempTransforms = std::move(m_transforms);
		for (auto* transform : tempTransforms)
		{
			insert(transform); // Reinsert will place it into the correct child
		}
	}


	void octree::redistribute()
	{
		// If this is a leaf node, it cannot redistribute its objects.
		if (m_is_leaf) return;

		// Temporarily store objects to be redistributed.
		std::vector<transform_component*> temp_transforms = std::move(m_transforms);

		// Insert each object into the appropriate child node.
		for (auto* transform : temp_transforms)
		{
			insert(transform);
		}

		// Now, redistribute each child node.
		for (auto& child : m_children)
		{
			if (child)
			{
				child->redistribute();
			}
		}
	}


	void octree::insert(transform_component* transform)
	{
		math::vec3 position = math::vec3(transform->m_transform[3]);
		ensure_contains(position); // This should grow the octree and subdivide as necessary

		if (m_is_leaf)
		{
			// This is a leaf node, so we can add the transform directly to this node
			if (m_transforms.size() < m_max_components)
			{
				m_transforms.push_back(transform);
			}
			else
			{
				// Node is at capacity, so subdivide and then insert
				subdivide();
				int32_t child_index = determine_child(math::vec3(transform->m_transform[3]));

				// Check if the child exists before attempting to insert the transform
				if (m_children[child_index])
				{
					m_children[child_index]->insert(transform);
				}
				else
				{
					// If the child does not exist, we need to create it and then insert the transform
					m_children[child_index] = std::make_unique<octree>(calculate_child_center(child_index), m_size / 2.0f, m_depth + 1, this);
					m_children[child_index]->insert(transform);
				}
			}
		}
		else
		{
			int32_t child_index = determine_child(math::vec3(transform->m_transform[3]));

			// Check if the child exists before attempting to insert the transform
			if (m_children[child_index])
			{
				m_children[child_index]->insert(transform);
			}
			else
			{
				// If the child does not exist, we need to create it and then insert the transform
				m_children[child_index] = std::make_unique<octree>(calculate_child_center(child_index), m_size / 2.0f, m_depth + 1, this);
				m_children[child_index]->insert(transform);
			}
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
			math::vec3 closest_point;
			closest_point.x = plane.m_normal.x > 0 ? m_center.x - m_size / 2 : m_center.x + m_size / 2;
			closest_point.y = plane.m_normal.y > 0 ? m_center.y - m_size / 2 : m_center.y + m_size / 2;
			closest_point.z = plane.m_normal.z > 0 ? m_center.z - m_size / 2 : m_center.z + m_size / 2;

			if (plane.get_signed_distance(closest_point) > 0)
			{
				return false; // AABB is completely outside the frustum
			}
		}
		return true; // Intersects or is completely inside the frustum
	}

	void octree::get_node_edges(std::vector<math::vec3>& lines) const
	{
		math::vec3 min_corner = m_center - math::vec3(m_size / 2.0f);
		math::vec3 max_corner = m_center + math::vec3(m_size / 2.0f);

		// 8 corners of the AABB
		math::vec3 corners[8] = {
			math::vec3(min_corner.x, min_corner.y, min_corner.z),
			math::vec3(max_corner.x, min_corner.y, min_corner.z),
			math::vec3(max_corner.x, max_corner.y, min_corner.z),
			math::vec3(min_corner.x, max_corner.y, min_corner.z),
			math::vec3(min_corner.x, min_corner.y, max_corner.z),
			math::vec3(max_corner.x, min_corner.y, max_corner.z),
			math::vec3(max_corner.x, max_corner.y, max_corner.z),
			math::vec3(min_corner.x, max_corner.y, max_corner.z)
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