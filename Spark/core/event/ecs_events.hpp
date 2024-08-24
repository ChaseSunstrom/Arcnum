#ifndef SPARK_ECS_EVENTS_HPP
#define SPARK_ECS_EVENTS_HPP

#include <core/pch.hpp>
#include <core/event/event.hpp>
#include <core/ecs/entity.hpp>
#include <core/ecs/component.hpp>

namespace Spark
{
	struct EntityCreatedEvent : public Event
	{
		EntityCreatedEvent(const Entity& entity) : Event(EVENT_TYPE_ENTITY_CREATED), entity(entity) {}
		const Entity& entity;
	};

	struct EntityDestroyedEvent : public Event
	{
		EntityDestroyedEvent(const Entity& entity) : Event(EVENT_TYPE_ENTITY_DESTROYED), entity(entity) {}
		const Entity& entity;
	};

	struct ComponentAddedEvent : public Event
	{
		template<typename T>
		ComponentAddedEvent(const Entity& entity, const T& component)
			: Event(EVENT_TYPE_COMPONENT_ADDED)
			, entity(entity)
			, component_type(decltype(T))
			, component_ptr(static_cast<const Component*>(&component))
		{}

		const Entity& entity;
		const std::type_index component_type;
		const Component* component_ptr;

		template<typename T>
		const T& get_component() const {
#ifdef __DEBUG__
			if (decltype(T) != component_type) {
				LOG_ERROR("Component type mismatch!");
				assert(false);
			}
#endif
			return *static_cast<const T*>(component_ptr);
		}
	};

	class ComponentRemovedEvent : public Event
	{
	public:
		template<typename T>
		ComponentRemovedEvent(const Entity& entity, const T& component)
			: Event(EVENT_TYPE_COMPONENT_REMOVED)
			, entity(entity)
			, component_type(decltype(T))
			, component_ptr(static_cast<const Component*>(&component))
		{}

		template<typename T>
		const T& get_component() const {
#ifdef __DEBUG__
			if (decltype(T) != component_type) {
				LOG_ERROR("Component type mismatch!");
				assert(false);
			}
#endif
			return *static_cast<const T*>(component_ptr);
		}

		const Entity& entity;
		const std::type_index component_type;
		const Component* component_ptr;
	};
}

#endif