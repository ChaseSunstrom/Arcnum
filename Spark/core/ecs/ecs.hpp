#ifndef SPARK_ECS_HPP
#define SPARK_ECS_HPP

#include <core/event/event.hpp>
#include <core/event/event_handler.hpp>
#include <core/pch.hpp>
#include <core/util/memory/ref_ptr.hpp>
#include "component.hpp"
#include "entity.hpp"
#include "system.hpp"

namespace Spark {
	class Ecs {
	public:
		Ecs(EventHandler& event_handler) : m_event_handler(event_handler) {}
		~Ecs() = default;

		Ecs(const Ecs&)            = delete;
		Ecs& operator=(const Ecs&) = delete;

		void Start();
		void Update(f32 delta_time);
		void Shutdown();

		template<IsComponent... Ts> RefPtr<Entity> MakeEntity(const std::pair<const char*, Ts>&... components);
		RefPtr<Entity>                             GetEntity(const u32 id) const;
		i64                                        GetEntityCount() const;
		void                                       DestroyEntity(const u32 id);

		template<IsComponent T> void AddComponent(RefPtr<Entity> entity, const std::string& name, const T& component);

		template<IsSystem T> void AddSystem(std::unique_ptr<T> system);

		template<IsComponent T> void RemoveComponents(RefPtr<Entity> entity);

		void RemoveAllEntityComponents(RefPtr<Entity> entity);
		void RemoveAllEntityComponents(const u32 id);

		template<IsComponent T> i64 GetComponentCount();

		template<IsComponent T> T& GetComponent(RefPtr<Entity> entity, const std::string& name);

		template<IsComponent T> T& GetComponent(const u32 id, const std::string& name);

		template<IsComponent T> Query<T>& GetComponents();

	private:
		template<IsComponent T> ComponentArray<T>& GetComponentArray();

	private:
		std::vector<Entity>                                                   m_entities;
		std::vector<std::unique_ptr<System>>                                  m_systems;
		std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> m_components;
		std::stack<u32>                                                       m_recycled_ids;
		EventHandler&                                                         m_event_handler;
	};

	template<IsComponent T> T& Ecs::GetComponent(RefPtr<Entity> entity, const std::string& name) {
		auto& component_array = GetComponentArray<T>();
		return component_array.GetData(entity->GetId());
	}

	template<IsComponent T> T& Ecs::GetComponent(const u32 id, const std::string& name) {
		auto& component_array = GetComponentArray<T>();
		return component_array.GetData(id);
	}

	template<IsComponent T> ComponentArray<T>& Ecs::GetComponentArray() {
		auto& ptr = m_components[typeid(T)];
		if (!ptr) { ptr = std::make_unique<ComponentArray<T>>(); }
		return *static_cast<ComponentArray<T>*>(ptr.get());
	}

	template<IsComponent T> i64 Ecs::GetComponentCount() { return GetComponentArray<T>().GetAllComponents().size(); }

	template<IsComponent... Ts> RefPtr<Entity> Ecs::MakeEntity(const std::pair<const char*, Ts>&... components) {
		u32 id = 0;
		if (!m_recycled_ids.empty()) { id = m_recycled_ids.top(); } else
			if (!m_entities.empty()) { id = m_entities.back().GetId() + 1; }

		Entity entity(id);
		m_entities.push_back(entity);

		m_event_handler.PublishEvent<EntityCreatedEvent>(MakeEvent<EntityCreatedEvent>(entity));

		(AddComponent(entity, components.first, components.second), ...);

		return m_entities.back();
	}

	template<IsComponent T> void Ecs::AddComponent(RefPtr<Entity> entity, const std::string& name, const T& component) {
		auto& component_array = GetComponentArray<T>();
		component_array.InsertData(entity->GetId(), component);
		entity->AddComponent<T>(name);
		m_event_handler.PublishEvent<ComponentAddedEvent<T>>(MakeEvent<ComponentAddedEvent<T>>(entity, ComponentEventType::ADDED, component));
	}

	template<IsSystem T> void Ecs::AddSystem(std::unique_ptr<T> system) { m_systems.push_back(std::move(system)); }

	template<IsComponent T> void Ecs::RemoveComponents(RefPtr<Entity> entity) {
		auto& component_array = GetComponentArray<T>();
		component_array.RemoveEntity(entity->GetId());
		entity->RemoveComponents<T>();
	}

	template<IsComponent T> Query<T>& Ecs::GetComponents() { return GetComponentArray<T>().GetAllComponents(); }
} // namespace Spark

#endif