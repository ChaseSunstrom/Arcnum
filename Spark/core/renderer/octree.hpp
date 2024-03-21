#ifndef SPARK_OCTREE_HPP
#define SPARK_OCTREE_HPP

#include "frustum.hpp"

#include "../spark.hpp"
#include "../ecs/ecs.hpp"

namespace spark
{
	struct octree
	{
		octree(const math::vec3& center, float32_t size) : m_center(center), m_size(size) {}
        octree(const octree& other)
            : m_center(other.m_center), m_size(other.m_size), m_is_leaf(other.m_is_leaf), m_max_components(other.m_max_components), m_max_depth(other.m_max_depth)
        {
            // Copy transforms
            for (auto* transform : other.m_transforms)
            {
                m_transforms.push_back(new transform_component(*transform)); // Assuming transform_component has a copy constructor
            }

            // Copy children
            for (size_t i = 0; i < other.m_children.size(); ++i)
            {
                if (other.m_children[i])
                {
                    m_children[i] = std::make_unique<octree>(*other.m_children[i]); // Recursively copy children
                }
                else
                {
                    m_children[i] = nullptr;
                }
            }
        }

        // Assignment operator
        octree& operator=(const octree& other)
        {
            if (this != &other)
            {
                // Copy member variables
                m_center = other.m_center;
                m_size = other.m_size;
                m_is_leaf = other.m_is_leaf;

                // Clear existing transforms
                for (auto* transform : m_transforms)
                {
                    delete transform;
                }
                m_transforms.clear();

                // Copy transforms
                for (auto* transform : other.m_transforms)
                {
                    m_transforms.push_back(new transform_component(*transform)); // Assuming transform_component has a copy constructor
                }

                // Clear existing children
                for (auto& child : m_children)
                {
                    child.reset();
                }

                // Copy children
                for (size_t i = 0; i < other.m_children.size(); ++i)
                {
                    if (other.m_children[i])
                    {
                        m_children[i] = std::make_unique<octree>(*other.m_children[i]); // Recursively copy children
                    }
                    else
                    {
                        m_children[i] = nullptr;
                    }
                }
            }
            return *this;
        }

		void insert(transform_component* transform);
		void subdivide();
        void redistribute();
		void ensure_contains(const math::vec3& point);
		bool is_inside(const math::vec3& point) const;
		bool is_inside(const math::vec3& point, float32_t size) const;
		bool is_inside(const math::vec3& point, const math::vec3& min, const math::vec3& max) const;
		int32_t determine_child(const math::vec3& point) const;
		int32_t determine_child(const math::vec3& point, const math::vec3& center) const;
		void collect_visible(const frustum& frustum, std::vector<transform_component*>& visible) const;
		void grow(const math::vec3& position);
		math::vec3 calculate_child_center(int32_t child_index) const;

		math::vec3 m_center;
		float32_t m_size;
		std::vector<transform_component*> m_transforms;
		std::array<std::unique_ptr<octree>, 8> m_children;
		bool m_is_leaf = false;

		const int32_t m_max_components = 8;
		const int32_t m_max_depth = 8;
	};
}

#endif