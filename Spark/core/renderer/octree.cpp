#include "octree.hpp"

namespace spark
{
	void octree::ensure_contains(const math::vec3& point)
	{
		if (is_inside(point)) return;

		int32_t levels_to_grow = 0;
		math::vec3 min = m_center - math::vec3(m_size / 2.0f);
		math::vec3 max = m_center + math::vec3(m_size / 2.0f);
		math::vec3 new_min = min;
		math::vec3 new_max = max;

		while (!is_inside(point, new_min, new_max))
		{
			m_size *= 2.0f;
			levels_to_grow++;

			for (int32_t i = 0; i < 3; i++)
			{
				if (point[i] < new_min[i])
					new_min[i] = m_center[i] - m_size/ 2.0f;

				else
					new_max[i] = m_center[i] + m_size / 2.0f;
			}
		}

		for (int32_t i = 0; i < levels_to_grow; i++)
			grow(point);
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
		if (!m_is_leaf) return;

		m_is_leaf = true;
		float32_t half_size = m_size / 2.0f;
		float32_t quarter_size = half_size / 2.0f;

		for (int32_t i = 0; i < 8; i++)
		{
			math::vec3 child_center = m_center + math::vec3(
				(i & 1 ? quarter_size : -quarter_size),
				(i & 2 ? quarter_size : -quarter_size),
				(i & 4 ? quarter_size : -quarter_size));
			m_children[i] = std::make_unique<octree>(child_center, half_size);
		}

		redistribute(); // Move existing transforms to appropriate child nodes

		m_transforms.clear(); // Clear transforms from this node
	}

	void octree::redistribute()
	{
		for (auto* transform : m_transforms)
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
		{
			ensure_contains(position);
			return;
		}

		if (m_is_leaf && m_transforms.size() < m_max_components)
		{
			m_transforms.emplace_back(transform);
		}
		else
		{
			if (m_is_leaf)
			{
				subdivide();
			}

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

	void octree::collect_visible(const frustum& frustum, std::vector<transform_component*>& transforms) const
	{
		if (!frustum.is_inside(m_center, m_size / 2.0f)) return;

		for (auto* transform : m_transforms)
			transforms.emplace_back(transform);

		if (!m_is_leaf) 
			for (auto& child : m_children) 
				child->collect_visible(frustum, transforms);
	}
}