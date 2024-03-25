#ifndef SPARK_OCTREE_HPP
#define SPARK_OCTREE_HPP

#include "frustum.hpp"

#include "../spark.hpp"
#include "../ecs/ecs.hpp"

namespace spark
{
	struct octree
	{
		octree(const math::vec3& center, float32_t size, int32_t depth = 0, octree* parent = nullptr) :
			m_center(center), m_size(size), m_is_leaf(true), m_depth(depth), m_parent(parent)
		{}

		void insert(transform_component* transform);
		void add_all_transforms(std::vector<math::mat4>& transforms) const;
		void subdivide();
		void adjust_children();
		void redistribute();
		void merge(std::unique_ptr<octree>&& old_child);
		void redistribute_to_children();
		void reposition_children();
		void redistribute_if_necessary();
		void redistribute_to_parent(); 
		math::vec3 calculate_min_point() const;
		math::vec3 calculate_max_point() const;
		void find_min_max_points(math::vec3& min_point, math::vec3& max_point) const;
		float calculate_new_size(const math::vec3& point) const;
		math::vec3 calculate_new_center(const math::vec3& point, float new_size) const;
		void get_node_edges(std::vector<glm::vec3>& lines) const;
		void ensure_contains(const math::vec3& point);
		bool is_inside(const math::vec3& point) const;
		bool is_inside(const math::vec3& point, const math::vec3& center, float half_size) const;
		bool is_inside(const math::vec3& point, float32_t size) const;
		bool is_inside(const math::vec3& point, const math::vec3& min, const math::vec3& max) const;
		bool is_inside(const math::vec3& point, float size, const math::vec3& center, float half_size) const;
		bool intersects(const frustum& frustum) const;
		bool contains_frustum(const frustum& frustum) const;
		int32_t determine_child(const math::vec3& point) const;
		int32_t determine_child(const math::vec3& point, const math::vec3& center) const;
		void collect_visible(const frustum& frustum, std::vector<math::mat4>& visible) const;
		void grow(const math::vec3& position);
		math::vec3 calculate_child_center(int32_t child_index) const;
		math::vec3 calculate_nearest_octant_center(const math::vec3& point) const;

		math::vec3 m_center;
		float32_t m_size;
		octree* m_parent;
		std::vector<transform_component*> m_transforms;
		std::array<std::unique_ptr<octree>, 8> m_children;
		bool m_is_leaf;
		int32_t m_depth;

		const int32_t m_max_components = 8;
		const int32_t m_max_depth = 8;
	};
}

#endif