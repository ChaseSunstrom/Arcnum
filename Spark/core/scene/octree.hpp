#ifndef SPARK_OCTREE_HPP
#define SPARK_OCTREE_HPP

#include <core/pch.hpp>
#include <include/glm/vec3.hpp>

namespace Spark
{
	class Octree
	{
	public:
		Octree(glm::vec3 center, f32 width) : m_root(std::make_unique<Node>(center, width / 2)) {
			if (width > MAX_SIZE) {
				LOG_ERROR("Initial octree size exceeds maximum allowed size");
				assert(false);
			}
		}

		void Insert(const glm::vec3& point) {
			if (!IsPointInBounds(point, m_root->center, m_root->half_width)) {
				if (m_root->half_width * 2 > MAX_SIZE) {
					LOG_ERROR("Cannot grow octree beyond maximum size");
					assert(false);
				}
				Grow(point);
			}
			Insert(m_root.get(), point, 0);
		}

		void Grow(const glm::vec3& new_point) {
			glm::vec3 new_center = m_root->center;
			f32 new_half_width = m_root->half_width * 2;

			while (!IsPointInBounds(new_point, new_center, new_half_width)) {
				if (new_half_width * 2 > MAX_SIZE) {
					LOG_ERROR("Cannot grow octree beyond maximum size");
					assert(false);
				}
				new_half_width *= 2;
				new_center.x += (new_point.x > new_center.x) ? new_half_width / 2 : -new_half_width / 2;
				new_center.y += (new_point.y > new_center.y) ? new_half_width / 2 : -new_half_width / 2;
				new_center.z += (new_point.z > new_center.z) ? new_half_width / 2 : -new_half_width / 2;
			}

			auto new_root = std::make_unique<Node>(new_center, new_half_width);
			i32 index = 0;
			if (m_root->center.x > new_center.x) index |= 1;
			if (m_root->center.y > new_center.y) index |= 2;
			if (m_root->center.z > new_center.z) index |= 4;
			new_root->children[index] = std::move(m_root);
			m_root = std::move(new_root);
		}

		void Shrink() {
			while (m_root->children[0]) {
				i32 non_null_count = 0;
				i32 last_non_null_index = -1;
				for (i32 i = 0; i < 8; ++i) {
					if (m_root->children[i]) {
						non_null_count++;
						last_non_null_index = i;
					}
				}

				if (non_null_count != 1) break;

				m_root = std::move(m_root->children[last_non_null_index]);
			}
		}

		void Rebuild() {
			std::vector<glm::vec3> all_points;
			CollectAllPoints(m_root.get(), all_points);

			if (all_points.empty()) return;

			glm::vec3 min_point = all_points[0], max_point = all_points[0];
			for (const auto& point : all_points) {
				min_point.x = std::min(min_point.x, point.x);
				min_point.y = std::min(min_point.y, point.y);
				min_point.z = std::min(min_point.z, point.z);
				max_point.x = std::max(max_point.x, point.x);
				max_point.y = std::max(max_point.y, point.y);
				max_point.z = std::max(max_point.z, point.z);
			}

			f32 max_dimension = std::max({ max_point.x - min_point.x, max_point.y - min_point.y, max_point.z - min_point.z });
			glm::vec3 center = {
				(min_point.x + max_point.x) / 2,
				(min_point.y + max_point.y) / 2,
				(min_point.z + max_point.z) / 2
			};

			m_root = std::make_unique<Node>(center, max_dimension / 2);

			for (const auto& point : all_points) {
				Insert(m_root.get(), point, 0);
			}
		}

	private:
		struct Node
		{
			Node(glm::vec3 c, f32 hw) : center(c), half_width(hw) {}

			glm::vec3 center;
			f32 half_width;
			std::vector<glm::vec3> points;
			std::array<std::unique_ptr<Node>, 8> children;
		};

		void Insert(Node* node, const glm::vec3& point, i32 depth) {
			if (depth >= MAX_DEPTH || node->points.size() < MAX_POINTS) {
				node->points.push_back(point);
				return;
			}

			if (!node->children[0]) {
				f32 hw = node->half_width / 2;
				for (i32 i = 0; i < 8; ++i) {
					f32 x = node->center.x + (i & 1 ? hw : -hw);
					f32 y = node->center.y + (i & 2 ? hw : -hw);
					f32 z = node->center.z + (i & 4 ? hw : -hw);
					node->children[i] = std::make_unique<Node>(glm::vec3{ x, y, z }, hw);
				}

				for (const auto& p : node->points) {
					Insert(node, p, depth + 1);
				}
				node->points.clear();
			}

			i32 index = 0;
			if (point.x > node->center.x) index |= 1;
			if (point.y > node->center.y) index |= 2;
			if (point.z > node->center.z) index |= 4;

			Insert(node->children[index].get(), point, depth + 1);
		}

		void CollectAllPoints(const Node* node, std::vector<glm::vec3>& all_points) const {
			if (!node) return;

			all_points.insert(all_points.end(), node->points.begin(), node->points.end());

			for (const auto& child : node->children) {
				CollectAllPoints(child.get(), all_points);
			}
		}

		bool IsPointInBounds(const glm::vec3& point, const glm::vec3& center, f32 half_width) const {
			return point.x >= center.x - half_width && point.x <= center.x + half_width &&
				point.y >= center.y - half_width && point.y <= center.y + half_width &&
				point.z >= center.z - half_width && point.z <= center.z + half_width;
		}
	private:
		static const i32 MAX_POINTS = 4;
		static const i32 MAX_DEPTH = 8;
		static const f32 MAX_SIZE;  // Maximum size limit
		std::unique_ptr<Node> m_root;
	};

	const f32 Octree::MAX_SIZE = 1000000.0f;
}

#endif