#include "ecs.hpp"

namespace Spark {

void Ecs::Start() {
	for (const auto& sys : m_systems) {
		sys->Start();
	}
}

void Ecs::Update() {
	for (const auto& sys : m_systems) {
		sys->Update();
	}
}

void Ecs::Shutdown() {
	for (const auto& sys : m_systems) {
		sys->Shutdown();
	}
}

Entity& Ecs::GetEntity(const i64 id) const { return *m_entities[id]; }

i64 Ecs::GetEntityCount() const { return m_entities.size(); }

void Ecs::DestroyEntity(const i64 id) {
	if (id < 0 || id >= m_entities.size()) {
		// Invalid ID, possibly already removed
		return;
	}

	RemoveAllEntityComponents(id);

	// Use swap-and-pop instead of erase to avoid shifting elements
	if (id != m_entities.size() - 1) {
		std::swap(m_entities[id], m_entities.back());
		m_entities[id]->SetId(id); // Update the swapped entity's ID
	}
	m_entities.pop_back();

	Entity::s_old_ids.push(id);
}

void Ecs::RemoveAllEntityComponents(Entity& entity) { RemoveAllEntityComponents(entity.GetId()); }

void Ecs::RemoveAllEntityComponents(const i64 id) {
	// First, remove components from the entity
	Entity& entity = GetEntity(id);
	entity.RemoveAllComponents();

	// Then, remove components from the ECS storage
	for (auto& [type, components] : m_components) {
		// Store components to be removed
		std::vector<std::shared_ptr<Component>> removed_components;

		// Identify components to be removed
		auto it = std::remove_if(components.begin(), components.end(), [id, &removed_components](const std::shared_ptr<Component>& component) {
			if (component->GetEntityId() == id) {
				removed_components.push_back(component);
				return true;
			}
			return false;
		});

		// Erase the removed elements
		components.erase(it, components.end());

		// Publish events for each removed component
		for (const auto& component : removed_components) {
			m_event_handler.PublishEvent<ComponentRemovedEvent<Component>>(std::make_shared<ComponentRemovedEvent<Component>>(entity, ComponentEventType::REMOVED, component));
		}
	}
}
} // namespace Spark