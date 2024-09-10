#include "ecs.hpp"

namespace Spark {

	void Ecs::Start() {
		for (const auto& sys : m_systems) {
			sys->Start();
		}
	}

	void Ecs::Update(f32 delta_time) {
		for (const auto& sys : m_systems) {
			sys->Update(delta_time);
		}
	}

	void Ecs::Shutdown() {
		for (const auto& sys : m_systems) {
			sys->Shutdown();
		}
	}

	RefPtr<Entity> Ecs::GetEntity(const u32 id) const {
		auto it = std::find_if(m_entities.Begin(), m_entities.End(), [id](const Entity& e) { return e.GetId() == id; });
		if (it != m_entities.End()) {
			return const_cast<Entity&>(*it);
		}
		LOG_FATAL("Entity not found");
	}

	i64 Ecs::GetEntityCount() const { return m_entities.Size(); }

	void Ecs::DestroyEntity(const u32 id) {
		auto it = std::find_if(m_entities.Begin(), m_entities.End(), [id](const Entity& e) { return e.GetId() == id; });
		if (it == m_entities.End()) {
			// Invalid ID, possibly already removed
			return;
		}

		RemoveAllEntityComponents(id);

		// Use swap-and-pop instead of erase to avoid shifting elements
		if (it != m_entities.end() - 1) {
			Swap(*it, m_entities.Back());
		}

		u32 recycled_id = m_entities.Back().GetId();
		m_entities.PopBack();
		m_recycled_ids.Push(recycled_id);
	}

	void Ecs::RemoveAllEntityComponents(RefPtr<Entity> entity) { RemoveAllEntityComponents(entity->GetId()); }

	void Ecs::RemoveAllEntityComponents(const u32 id) {
		// Remove components from the entity
		auto entity = GetEntity(id);
		entity->RemoveAllComponents();

		// Remove components from the ECS storage
		for (auto& [type, component_array] : m_components) {
			component_array->RemoveEntity(id);
		}
	}
} // namespace Spark