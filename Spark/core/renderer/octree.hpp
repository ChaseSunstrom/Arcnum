#ifndef SPARK_OCTREE_HPP
#define SPARK_OCTREE_HPP

#include "frustum.hpp"

#include "../spark.hpp"
#include "../ecs/ecs.hpp"

namespace spark
{
	struct octree
	{
		octree(const math::vec3& center, float32_t size, int32_t depth = 0) :
			m_center(center), m_size(size), m_is_leaf(true), m_depth(depth)
		{}

		void insert(transform_component* transform);
		void add_all_transforms(std::vector<math::mat4>& transforms) const;
		void subdivide();
		void redistribute();
		void get_node_edges(std::vector<glm::vec3>& lines) const;
		void ensure_contains(const math::vec3& point);
		bool is_inside(const math::vec3& point) const;
		bool is_inside(const math::vec3& point, float32_t size) const;
		bool is_inside(const math::vec3& point, const math::vec3& min, const math::vec3& max) const;
		bool intersects(const frustum& frustum) const;
		bool contains_frustum(const frustum& frustum) const;
		int32_t determine_child(const math::vec3& point) const;
		int32_t determine_child(const math::vec3& point, const math::vec3& center) const;
		void collect_visible(const frustum& frustum, std::vector<math::mat4>& visible) const;
		void grow(const math::vec3& position);
		math::vec3 calculate_child_center(int32_t child_index) const;

		math::vec3 m_center;
		float32_t m_size;
		std::vector<transform_component*> m_transforms;
		std::array<std::unique_ptr<octree>, 8> m_children;
		bool m_is_leaf;
		int32_t m_depth;

		const int32_t m_max_components = 8;
		const int32_t m_max_depth = 8;
	};
}

#endif