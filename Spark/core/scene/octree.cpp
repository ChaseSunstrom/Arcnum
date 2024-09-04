#include "octree.hpp"
#include <core/ecs/components/transform_component.hpp>
#include <core/event/event.hpp>
#include <include/glm/glm.hpp>
#include <include/glm/gtx/component_wise.hpp>
#include <include/glm/vector_relational.hpp>

namespace Spark {
Octree::Octree(glm::vec3 center, f32 width)
	: m_root(std::make_unique<Node>(center, width / 2)) {
	if (width > MAX_SIZE) {
		LOG_FATAL("Initial octree size exceeds maximum allowed size");
	}
}

void Octree::OnEvent(const std::shared_ptr<ComponentEvent<TransformComponent>> event) {
	switch (event->type) {
		case ComponentEventType::ADDED: {
			Insert({event->component->transform.GetPosition(), event->entity.GetId()});
			break;
		}
		case ComponentEventType::REMOVED: {
			Update({event->component->transform.GetPosition(), event->entity.GetId()});
			break;
		}
		case ComponentEventType::UPDATED: {
			Remove(event->entity.GetId());
			break;
		}
	}
}

void Octree::Insert(const PointData& point_data) {
	if (!IsPointInBounds(point_data.position, m_root->center, m_root->half_width)) {
		if (m_root->half_width * 2 > MAX_SIZE) {
			LOG_FATAL("Cannot grow octree beyond maximum size");
		}
		Grow(point_data.position);
	}
	Node* node                             = Insert(m_root.get(), point_data, 0);
	m_entity_to_node[point_data.entity_id] = node;
}

bool Octree::Remove(i64 entity_id) {
	auto it = m_entity_to_node.find(entity_id);
	if (it == m_entity_to_node.end())
		return false;

	Node* node   = it->second;
	auto& points = node->points;
	points.erase(std::remove_if(points.begin(), points.end(), [entity_id](const PointData& pd) { return pd.entity_id == entity_id; }), points.end());

	m_entity_to_node.erase(it);

	return true;
}

bool Octree::Update(const PointData& point_data) {
	if (Remove(point_data.entity_id)) {
		Insert({point_data.position, point_data.entity_id});
		return true;
	}
	return false;
}

void Octree::Grow(const glm::vec3& new_point) {
	glm::vec3 new_center = m_root->center;
	f32 new_half_width   = m_root->half_width * 2;

	while (!IsPointInBounds(new_point, new_center, new_half_width)) {
		if (new_half_width * 2 > MAX_SIZE) {
			LOG_FATAL("Cannot grow octree beyond maximum size");
		}
		new_half_width *= 2;
		new_center.x += (new_point.x > new_center.x) ? new_half_width / 2 : -new_half_width / 2;
		new_center.y += (new_point.y > new_center.y) ? new_half_width / 2 : -new_half_width / 2;
		new_center.z += (new_point.z > new_center.z) ? new_half_width / 2 : -new_half_width / 2;
	}

	auto new_root = std::make_unique<Node>(new_center, new_half_width);
	i32 index     = 0;
	if (m_root->center.x > new_center.x)
		index |= 1;
	if (m_root->center.y > new_center.y)
		index |= 2;
	if (m_root->center.z > new_center.z)
		index |= 4;
	new_root->children[index] = std::move(m_root);
	m_root                    = std::move(new_root);
}

void Octree::Shrink() {
	while (m_root->children[0]) {
		i32 non_null_count      = 0;
		i32 last_non_null_index = -1;
		for (i32 i = 0; i < 8; ++i) {
			if (m_root->children[i]) {
				non_null_count++;
				last_non_null_index = i;
			}
		}

		if (non_null_count != 1)
			break;

		m_root = std::move(m_root->children[last_non_null_index]);
	}
}

void Octree::Rebuild() {
	std::vector<PointData> all_points;
	CollectAllPoints(m_root.get(), all_points);

	if (all_points.empty())
		return;

	glm::vec3 min_point = all_points[0].position, max_point = all_points[0].position;
	for (const auto& point_data : all_points) {
		const auto& pos = point_data.position;
		min_point       = glm::min(min_point, pos);
		max_point       = glm::max(max_point, pos);
	}

	f32 max_dimension = glm::compMax(max_point - min_point);
	glm::vec3 center  = (min_point + max_point) * 0.5f;

	m_root            = std::make_unique<Node>(center, max_dimension / 2);
	m_entity_to_node.clear();
	for (const auto& point_data : all_points) {
		Node* node                             = Insert(m_root.get(), point_data, 0);
		m_entity_to_node[point_data.entity_id] = node;
	}
}

Octree::Node* Octree::Insert(Octree::Node* node, const PointData& point_data, i32 depth) {
	if (depth >= MAX_DEPTH || node->points.size() < MAX_POINTS) {
		node->points.push_back(point_data);
		return node;
	}

	if (!node->children[0]) {
		f32 hw = node->half_width / 2;
		for (i32 i = 0; i < 8; ++i) {
			f32 x             = node->center.x + (i & 1 ? hw : -hw);
			f32 y             = node->center.y + (i & 2 ? hw : -hw);
			f32 z             = node->center.z + (i & 4 ? hw : -hw);
			node->children[i] = std::make_unique<Node>(glm::vec3{x, y, z}, hw);
		}

		for (const auto& p : node->points) {
			Insert(node, p, depth + 1);
		}
		node->points.clear();
	}

	i32 index = 0;
	if (point_data.position.x > node->center.x)
		index |= 1;
	if (point_data.position.y > node->center.y)
		index |= 2;
	if (point_data.position.z > node->center.z)
		index |= 4;

	return Insert(node->children[index].get(), point_data, depth + 1);
}

void Octree::CollectAllPoints(const Node* node, std::vector<PointData>& all_points) const {
	if (!node)
		return;

	all_points.insert(all_points.end(), node->points.begin(), node->points.end());

	for (const auto& child : node->children) {
		CollectAllPoints(child.get(), all_points);
	}
}

std::vector<i64> Octree::GetEntitiesFromPoint(const glm::vec3& point) {
	std::vector<i64> result;
	Node* node = m_root.get();
	while (node) {
		if (!IsPointInBounds(point, node->center, node->half_width))
			break;

		for (const auto& point_data : node->points) {
			result.push_back(point_data.entity_id);
		}

		for (const auto& child : node->children) {
			node = child.get();
		}
	}

	return result;
}

bool Octree::IsPointInBounds(const glm::vec3& point, const glm::vec3& center, f32 half_width) const { return glm::all(glm::lessThanEqual(glm::abs(point - center), glm::vec3(half_width))); }

std::vector<PointData> Octree::QueryRange(const glm::vec3& min, const glm::vec3& max) {
	std::vector<PointData> result;
	QueryRange(m_root.get(), min, max, result);
	return result;
}

void Octree::QueryRange(Node* node, const glm::vec3& min, const glm::vec3& max, std::vector<PointData>& result) {
	if (!node)
		return;

	if (!IntersectsRange(node->center, node->half_width, min, max))
		return;

	for (const auto& point : node->points) {
		if (IsPointInRange(point.position, min, max)) {
			result.push_back(point);
		}
	}

	for (const auto& child : node->children) {
		QueryRange(child.get(), min, max, result);
	}
}

bool Octree::IntersectsRange(const glm::vec3& center, f32 half_width, const glm::vec3& min, const glm::vec3& max) {
	glm::vec3 box_min = center - glm::vec3(half_width);
	glm::vec3 box_max = center + glm::vec3(half_width);
	return glm::all(glm::lessThanEqual(box_min, max)) && glm::all(glm::greaterThanEqual(box_max, min));
}

bool Octree::IsPointInRange(const glm::vec3& point, const glm::vec3& min, const glm::vec3& max) { return glm::all(glm::greaterThanEqual(point, min)) && glm::all(glm::lessThanEqual(point, max)); }
} // namespace Spark