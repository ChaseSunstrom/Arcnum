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

		template<IsComponent _Ty> void AddComponent(RefPtr<Entity> entity, const String& name, const _Ty& component);

		template<IsSystem _Ty> void AddSystem(UniquePtr<_Ty> system);

		template<IsComponent _Ty> void RemoveComponents(RefPtr<Entity> entity);

		void RemoveAllEntityComponents(RefPtr<Entity> entity);
		void RemoveAllEntityComponents(const u32 id);

		template<IsComponent _Ty> i64 GetComponentCount();

		template<IsComponent _Ty> _Ty& GetComponent(RefPtr<Entity> entity, const String& name);

		template<IsComponent _Ty> _Ty& GetComponent(const u32 id, const String& name);

		template<IsComponent _Ty> Query<_Ty>& GetComponents();

	  private:
		template<IsComponent _Ty> ComponentArray<_Ty>& GetComponentArray();

	  private:
		Vector<Entity>                                      m_entities;
		Vector<UniquePtr<System>>                           m_systems;
		UnorderedMap<TypeIndex, UniquePtr<IComponentArray>> m_components;
		Stack<u32>                                          m_recycled_ids;
		RefPtr<EventHandler>                                m_event_handler;
	};

	template<IsComponent _Ty> _Ty& Ecs::GetComponent(RefPtr<Entity> entity, const String& name) {
		auto& component_array = GetComponentArray<_Ty>();
		return component_array.GetData(entity->GetId());
	}

	template<IsComponent _Ty> _Ty& Ecs::GetComponent(const u32 id, const String& name) {
		auto& component_array = GetComponentArray<_Ty>();
		return component_array.GetData(id);
	}

	template<IsComponent _Ty> ComponentArray<_Ty>& Ecs::GetComponentArray() {
		auto& ptr = m_components[typeid(_Ty)];
		if (!ptr) {
			ptr = MakeUnique<ComponentArray<_Ty>>();
		}
		return *static_cast<ComponentArray<_Ty>*>(ptr.get());
	}

	template<IsComponent _Ty> i64 Ecs::GetComponentCount() { return GetComponentArray<_Ty>().GetAllComponents().size(); }

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

	template<IsComponent _Ty> void Ecs::AddComponent(RefPtr<Entity> entity, const String& name, const _Ty& component) {
		auto& component_array = GetComponentArray<_Ty>();
		component_array.InsertData(entity->GetId(), component);
		entity->AddComponent<_Ty>(name);
		m_event_handler->PublishEvent<ComponentAddedEvent<_Ty>>(MakeEvent<ComponentAddedEvent<_Ty>>(entity, ComponentEventType::ADDED, component));
	}

	template<IsSystem _Ty> void Ecs::AddSystem(UniquePtr<_Ty> system) { m_systems.PushBack(Move(system)); }

	template<IsComponent _Ty> void Ecs::RemoveComponents(RefPtr<Entity> entity) {
		auto& component_array = GetComponentArray<_Ty>();
		component_array.RemoveEntity(entity->GetId());
		entity->RemoveComponents<_Ty>();
	}

	template<IsComponent _Ty> Query<_Ty>& Ecs::GetComponents() { return GetComponentArray<_Ty>().GetAllComponents(); }
} // namespace Spark

#endif