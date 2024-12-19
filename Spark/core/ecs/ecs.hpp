#ifndef SPARK_ECS_HPP
#define SPARK_ECS_HPP

#include <array>
#include <core/pch.hpp>
#include "query.hpp"
#include "entity.hpp"
#include "system.hpp"
#include <core/event/event.hpp>
#include <core/event/event_handler.hpp>

namespace Spark {
	// Archetype represents a unique combination of components
	class Archetype {
	  public:
		ComponentMask                      mask;
		Vector<RefPtr<Entity>>             entities;
		Vector<UniquePtr<IComponentArray>> components;

		bool Matches(const ComponentMask& required, const ComponentMask& excluded = ComponentMask()) const { return (mask & required) == required && (mask & excluded).none(); }
	};

	class EntityCache {
	  public:
		void Add(RefPtr<Entity> entity, u32 archetype_index) { m_entity_to_archetype[entity->GetId()] = archetype_index; }

		void Remove(u32 entity_id) { m_entity_to_archetype.Erase(entity_id); }

		u32 GetArchetypeIndex(u32 entity_id) const {
			auto it = m_entity_to_archetype.Find(entity_id);
			return it != m_entity_to_archetype.end() ? it->second : UINT32_MAX;
		}

	  private:
		UnorderedMap<u32, u32> m_entity_to_archetype;
	};

	class Ecs {
	  public:
		Ecs(RefPtr<EventHandler> event_handler)
			: m_event_handler(event_handler)
			, m_component_change_tracker(MAX_COMPONENTS) {}

		// Optimized query creation with archetype filtering
		template<typename... Components> Query<Components...> CreateQuery() {
			ComponentMask required_mask;
			(required_mask.set(GetComponentId<Components>()), ...);

			// Cache matching archetypes for this query
			Vector<u32> matching_indices;
			for (u32 i = 0; i < m_archetypes.Size(); ++i) {
				if (m_archetypes[i].Matches(required_mask)) {
					matching_indices.PushBack(i);
				}
			}

			return Query<Components...>(m_archetypes, matching_indices);
		}

		// Optimized entity creation with archetype management
		template<typename... Components> RefPtr<Entity> SpawnEntity(Components&&... components) {
			ComponentMask mask;
			(mask.set(GetComponentId<Components>()), ...);

			// Find or create matching archetype
			u32   archetype_index = GetOrCreateArchetype(mask);
			auto& archetype       = m_archetypes[archetype_index];

			// Create entity
			Entity entity(NextEntityId());
			auto   entity_ref = MakeRefPtr<Entity>(Move(entity));

			// Add components
			(AddComponentToArchetype<Components>(archetype, entity_ref, Forward<Components>(components)), ...);

			archetype.entities.PushBack(entity_ref);
			m_entity_cache.Add(entity_ref, archetype_index);

			m_event_handler->PublishEvent(MakeEvent<EntityCreatedEvent>(*entity_ref));
			return entity_ref;
		}

		// Fast component access with cache
		template<IsComponent T> T& GetComponent(RefPtr<Entity> entity) {
			u32   archetype_index = m_entity_cache.GetArchetypeIndex(entity->GetId());
			auto& archetype       = m_archetypes[archetype_index];
			u32   component_id    = GetComponentId<T>();
			auto& array           = static_cast<ComponentArray<T>&>(*archetype.components[component_id]);
			return array.GetData(entity->GetId());
		}

		// Efficient component addition with archetype transition
		template<typename T> void AddComponent(RefPtr<Entity> entity, T&& component) {
			u32   old_archetype_index = m_entity_cache.GetArchetypeIndex(entity->GetId());
			auto& old_archetype       = m_archetypes[old_archetype_index];

			// Create new archetype mask
			ComponentMask new_mask    = old_archetype.mask;
			new_mask.set(GetComponentId<T>());

			// Find or create new archetype
			u32   new_archetype_index = GetOrCreateArchetype(new_mask);
			auto& new_archetype       = m_archetypes[new_archetype_index];

			// Move entity and its components to new archetype
			MoveEntityToArchetype(entity, old_archetype_index, new_archetype_index);

			// Add new component
			AddComponentToArchetype<T>(new_archetype, entity, Forward<T>(component));

			m_event_handler->PublishEvent(MakeEvent<ComponentAddedEvent<T>>(*entity, ComponentEventType::ADDED, component));
		}

		// Batch operations
		template<typename T, typename Func> void UpdateComponents(Func&& update_func) {
			auto query = CreateQuery<T>();
			query.ForEach([&](T& component) {
				update_func(component);
				MarkComponentChanged<T>(component.GetEntityId());
			});
		}

		// Component change tracking
		template<typename T> bool WasComponentChanged(u32 entity_id) const {
			u32 component_id = GetComponentId<T>();
			return m_component_change_tracker[component_id].test(entity_id % 64);
		}

		void ClearChangeTracker() {
			for (auto& mask : m_component_change_tracker) {
				mask.reset();
			}
		}

		// Entity management
		void DestroyEntity(RefPtr<Entity> entity) {
			u32   archetype_index = m_entity_cache.GetArchetypeIndex(entity->GetId());
			auto& archetype       = m_archetypes[archetype_index];

			m_event_handler->PublishEvent(MakeEvent<EntityDestroyedEvent>(*entity));

			// Remove from archetype
			auto it = std::find(archetype.entities.Begin(), archetype.entities.End(), entity);
			if (it != archetype.entities.End()) {
				RemoveEntityFromArchetype(entity, archetype_index);
			}

			m_entity_cache.Remove(entity->GetId());
			m_recycled_ids.Push(entity->GetId());
		}

	  private:
		u32 NextEntityId() {
			if (!m_recycled_ids.Empty()) {
				u32 id = m_recycled_ids.Top();
				m_recycled_ids.Pop();
				return id;
			}
			return m_next_entity_id++;
		}

		u32 GetOrCreateArchetype(const ComponentMask& mask) {
			for (u32 i = 0; i < m_archetypes.Size(); ++i) {
				if (m_archetypes[i].mask == mask) {
					return i;
				}
			}

			Archetype new_archetype;
			new_archetype.mask = mask;
			new_archetype.components.Resize(MAX_COMPONENTS);
			m_archetypes.PushBack(Move(new_archetype));
			return m_archetypes.Size() - 1;
		}

		template<typename T> void AddComponentToArchetype(Archetype& archetype, RefPtr<Entity> entity, T&& component) {
			u32 component_id = GetComponentId<T>();
			if (!archetype.components[component_id]) {
				archetype.components[component_id] = MakeUnique<ComponentArray<T>>();
			}
			auto& array = static_cast<ComponentArray<T>&>(*archetype.components[component_id]);
			array.InsertData(entity->GetId(), Forward<T>(component));
			entity->AddComponent<T>();
		}

		void MoveEntityToArchetype(RefPtr<Entity> entity, u32 from_index, u32 to_index) {
			auto& from_archetype = m_archetypes[from_index];
			auto& to_archetype   = m_archetypes[to_index];

			// Move existing components
			for (u32 i = 0; i < MAX_COMPONENTS; ++i) {
				if (from_archetype.mask.test(i) && to_archetype.mask.test(i)) {
					from_archetype.components[i]->MoveEntity(entity->GetId(), *to_archetype.components[i]);
				}
			}

			// Update entity references
			auto it = std::find(from_archetype.entities.Begin(), from_archetype.entities.End(), entity);
			if (it != from_archetype.entities.End()) {
				from_archetype.entities.Erase(it);
			}
			to_archetype.entities.PushBack(entity);
			m_entity_cache.Add(entity, to_index);
		}

		template<typename T> void MarkComponentChanged(u32 entity_id) {
			u32 component_id = GetComponentId<T>();
			m_component_change_tracker[component_id].set(entity_id % 64);
		}

		void RemoveEntityFromArchetype(RefPtr<Entity> entity, u32 archetype_index) {
			auto& archetype = m_archetypes[archetype_index];
			for (auto& component_array : archetype.components) {
				if (component_array) {
					component_array->RemoveEntity(entity->GetId());
				}
			}
			auto it = std::find(archetype.entities.Begin(), archetype.entities.End(), entity);
			if (it != archetype.entities.End()) {
				archetype.entities.Erase(it);
			}
		}

		Vector<Archetype>       m_archetypes;
		EntityCache             m_entity_cache;
		RefPtr<EventHandler>    m_event_handler;
		Stack<u32>              m_recycled_ids;
		u32                     m_next_entity_id = 0;
		Vector<std::bitset<64>> m_component_change_tracker;
	};

} // namespace Spark

#endif