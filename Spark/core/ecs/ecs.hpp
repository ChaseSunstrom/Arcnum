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
		Ecs(RefPtr<EventHandler> event_handler)
			: m_event_handler(event_handler) {}
		~Ecs()                     = default;

		Ecs(const Ecs&)            = delete;
		Ecs& operator=(const Ecs&) = delete;

		void Start();
		void Update(f32 delta_time);
		void Shutdown();

		template<IsComponent... Ts> RefPtr<Entity> MakeEntity(const Pair<const char*, Ts>&... components);
		RefPtr<Entity>                             GetEntity(const u32 id) const;
		i64                                        GetEntityCount() const;
		void                                       DestroyEntity(const u32 id);

		template<IsComponent T> void AddComponent(RefPtr<Entity> entity, const String& name, const T& component);

		template<IsSystem T> void AddSystem(UniquePtr<T> system);

		template<IsComponent T> void RemoveComponents(RefPtr<Entity> entity);

		void RemoveAllEntityComponents(RefPtr<Entity> entity);
		void RemoveAllEntityComponents(const u32 id);

		template<IsComponent T> i64 GetComponentCount();

		template<IsComponent T> T& GetComponent(RefPtr<Entity> entity, const String& name);

		template<IsComponent T> T& GetComponent(const u32 id, const String& name);

		template<IsComponent T> Query<T>& GetComponents();

	  private:
		template<IsComponent T> ComponentArray<T>& GetComponentArray();

	  private:
		Vector<Entity>                                                        m_entities;
		Vector<UniquePtr<System>>                                       m_systems;
		UnorderedMap<std::type_index, UniquePtr<IComponentArray>> m_components;
		Stack<u32>                                                       m_recycled_ids;
		RefPtr<EventHandler>                                                         m_event_handler;
	};

	template<IsComponent T> T& Ecs::GetComponent(RefPtr<Entity> entity, const String& name) {
		auto& component_array = GetComponentArray<T>();
		return component_array.GetData(entity->GetId());
	}

	template<IsComponent T> T& Ecs::GetComponent(const u32 id, const String& name) {
		auto& component_array = GetComponentArray<T>();
		return component_array.GetData(id);
	}

	template<IsComponent T> ComponentArray<T>& Ecs::GetComponentArray() {
		auto& ptr = m_components[typeid(T)];
		if (!ptr) {
			ptr = MakeUnique<ComponentArray<T>>();
		}
		return *static_cast<ComponentArray<T>*>(ptr.get());
	}

	template<IsComponent T> i64 Ecs::GetComponentCount() { return GetComponentArray<T>().GetAllComponents().size(); }

	template<IsComponent... Ts> RefPtr<Entity> Ecs::MakeEntity(const Pair<const char*, Ts>&... components) {
		u32 id = 0;
		if (!m_recycled_ids.Empty()) {
			id = m_recycled_ids.Top();
		} else if (!m_entities.Empty()) {
			id = m_entities.Back().GetId() + 1;
		}

		Entity entity(id);
		m_entities.PushBack(entity);

		m_event_handler->PublishEvent<EntityCreatedEvent>(MakeEvent<EntityCreatedEvent>(entity));

		(AddComponent(entity, components.first, components.second), ...);

		return m_entities.Back();
	}

	template<IsComponent T> void Ecs::AddComponent(RefPtr<Entity> entity, const String& name, const T& component) {
		auto& component_array = GetComponentArray<T>();
		component_array.InsertData(entity->GetId(), component);
		entity->AddComponent<T>(name);
		m_event_handler->PublishEvent<ComponentAddedEvent<T>>(MakeEvent<ComponentAddedEvent<T>>(entity, ComponentEventType::ADDED, component));
	}

	template<IsSystem T> void Ecs::AddSystem(UniquePtr<T> system) { m_systems.PushBack(Move(system)); }

	template<IsComponent T> void Ecs::RemoveComponents(RefPtr<Entity> entity) {
		auto& component_array = GetComponentArray<T>();
		component_array.RemoveEntity(entity->GetId());
		entity->RemoveComponents<T>();
	}

	template<IsComponent T> Query<T>& Ecs::GetComponents() { return GetComponentArray<T>().GetAllComponents(); }
} // namespace Spark

#endif