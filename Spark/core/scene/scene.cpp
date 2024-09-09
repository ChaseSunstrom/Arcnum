#include "scene.hpp"

namespace Spark {

	void Scene::OnEvent(const std::shared_ptr<ComponentEvent<TransformComponent>> event) {
		// Update the Octree
		m_octree->OnEvent(event);

		// Update the model instance if it exists
		auto it = m_entity_to_instance_map.find(event->entity.GetId());
		if (it != m_entity_to_instance_map.end()) {
			const auto& [model_name, index]      = it->second;
			m_model_instances[model_name][index] = event->component.transform.GetTransform();
		}
	}

	void Scene::Insert(const PointData& point_data) { m_octree->Insert(point_data); }

	void Scene::Update(const PointData& point_data) { m_octree->Update(point_data); }

	void Scene::Remove(i64 entity_id) {
		m_octree->Remove(entity_id);

		auto it = m_entity_to_instance_map.find(entity_id);
		if (it != m_entity_to_instance_map.end()) {
			const auto& [model_name, index] = it->second;
			RemoveModelInstance(model_name, index);
			m_entity_to_instance_map.erase(it);
		}
	}

	void Scene::AddModelInstance(Entity& entity, const std::string& model_name, const Math::Mat4& transform) {
		m_model_instances[model_name].push_back(transform);
		size_t index = m_model_instances[model_name].size() - 1;

		m_entity_to_instance_map[entity.GetId()] = {model_name, index};
	}

	void Scene::UpdateModelInstance(const std::string& model_name, size_t index, const Math::Mat4& transform) {
		if (m_model_instances.count(model_name) > 0 && index < m_model_instances[model_name].size()) {
			m_model_instances[model_name][index] = transform;
		}
	}

	void Scene::RemoveModelInstance(const std::string& model_name, size_t index) {
		if (m_model_instances.count(model_name) > 0 && index < m_model_instances[model_name].size()) {
			m_model_instances[model_name].erase(m_model_instances[model_name].begin() + index);

			// Update indices in m_entity_to_instance_map
			for (auto& [entity_id, instance_info] : m_entity_to_instance_map) {
				if (instance_info.first == model_name && instance_info.second > index) {
					instance_info.second--;
				}
			}
		}
	}

} // namespace Spark