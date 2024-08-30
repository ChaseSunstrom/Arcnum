#include "scene.hpp"

namespace Spark {
void Scene::OnEvent(const std::shared_ptr<Event> event) {
	// The octree should not be registered to receive events, which is why we're sending them here
	m_octree->OnEvent(event);
}

void Scene::Insert(const PointData& point_data) {
	m_octree->Insert(point_data);
}

void Scene::Update(const PointData& point_data) {
	m_octree->Update(point_data);
}

void Scene::Remove(i64 entity_id) {
	m_octree->Remove(entity_id);
}
} // namespace Spark