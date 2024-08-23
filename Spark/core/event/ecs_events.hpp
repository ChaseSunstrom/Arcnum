#ifndef SPARK_ECS_EVENTS_HPP
#define SPARK_ECS_EVENTS_HPP

#include <core/pch.hpp>
#include <core/event/event.hpp>
#include <core/ecs/entity.hpp>
#include <core/ecs/component.hpp>

namespace Spark
{
	class EntityCreatedEvent : public Event
	{
	public:
		EntityCreatedEvent(const Entity& entity) : Event(EVENT_TYPE_ENTITY_CREATED), entity(entity) {}
	public:
		const Entity& entity;
	};

	class EntityDestroyedEvent : public Event
	{
	public:
		EntityDestroyedEvent(const Entity& entity) : Event(EVENT_TYPE_ENTITY_DESTROYED), entity(entity) {}
	public:
		const Entity& entity;
	};

	class ComponentAddedEvent : public Event
	{
	public:
		ComponentAddedEvent(const Entity& entity, const Component& component) : Event(EVENT_TYPE_COMPONENT_ADDED), entity(entity), component(component) {}
	public:
		const Entity& entity;
		const Component& component;
	};

	class ComponentRemovedEvent : public Event
	{
	public:
		ComponentRemovedEvent(const Entity& entity, const Component& component) : Event(EVENT_TYPE_COMPONENT_REMOVED), entity(entity), component(component) {}
	public:
		const Entity& entity;
		const Component& component;
	};
}


#endif