#ifndef SPARK_ECS_EVENTS_HPP
#define SPARK_ECS_EVENTS_HPP

#include <core/ecs/component.hpp>
#include <core/ecs/entity.hpp>
#include <core/event/event.hpp>
#include <core/pch.hpp>

namespace Spark {
struct EntityCreatedEvent : public Event {
	EntityCreatedEvent(const Entity& entity)
		: Event(EVENT_TYPE_ENTITY_CREATED)
		, entity(entity) {}
	const Entity& entity;
};

struct EntityDestroyedEvent : public Event {
	EntityDestroyedEvent(const Entity& entity)
		: Event(EVENT_TYPE_ENTITY_DESTROYED)
		, entity(entity) {}
	const Entity& entity;
};

struct ComponentAddedEvent : public Event {
	ComponentAddedEvent(const Entity& entity, const std::type_index& type, const std::shared_ptr<Component> component)
		: Event(EVENT_TYPE_COMPONENT_ADDED)
		, entity(entity)
#ifdef __DEBUG__
		, component_type(type)
#endif
		, component_ptr(component) {
	}

	template <IsComponent T> const T& GetComponent() const {
#ifdef __DEBUG__
		if (typeid(T) != component_type) {
			LOG_FATAL("Component type mismatch!");
		}
#endif
		return *std::static_pointer_cast<T>(component_ptr);
	}

	const Entity& entity;
#ifdef __DEBUG__
	const std::type_index component_type;
#endif
	const std::shared_ptr<Component> component_ptr;
};

struct ComponentRemovedEvent : public Event {
	ComponentRemovedEvent(const Entity& entity, const std::type_index& type, const std::shared_ptr<Component> component)
		: Event(EVENT_TYPE_COMPONENT_REMOVED)
		, entity(entity)
#ifdef __DEBUG__
		, component_type(type)
#endif
		, component_ptr(component) {
	}

	template <IsComponent T> const T& GetComponent() const {
#ifdef __DEBUG__
		if (typeid(T) != component_type) {
			LOG_FATAL("Component type mismatch!");
		}
#endif
		return *std::static_pointer_cast<T>(component_ptr);
	}

	const Entity& entity;
#ifdef __DEBUG__
	const std::type_index component_type;
#endif
	const std::shared_ptr<Component> component_ptr;
};
} // namespace Spark

#endif