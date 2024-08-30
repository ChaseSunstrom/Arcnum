#ifndef SPARK_ECS_HPP
#define SPARK_ECS_HPP

#include "component.hpp"
#include "entity.hpp"
#include <core/event/ecs_events.hpp>
#include <core/event/event_handler.hpp>
#include <core/pch.hpp>

namespace Spark
{

class Ecs
{
  public:
	Ecs(EventHandler &event_handler) : m_event_handler(event_handler) {}
	~Ecs() = default;

	Ecs(const Ecs &)			= delete;
	Ecs &operator=(const Ecs &) = delete;

	template <IsComponent... Ts> Entity &MakeEntity(const std::pair<const char *, Ts *> &...components);
	Entity								&GetEntity(const i64 id) const;
	i64									 GetEntityCount() const;
	void								 DestroyEntity(const i64 id);

	template <IsComponent T> void AddComponent(Entity &entity, const std::string &name, T *component);

	template <IsComponent T> void AddComponent(Entity &entity, const std::string &name, std::shared_ptr<T> component);

	template <IsComponent T> void RemoveComponents(Entity &entity);

	void RemoveAllEntityComponents(Entity &entity);
	void RemoveAllEntityComponents(const i64 id);

	template <IsComponent T> Query<T> &GetComponents();

	template <IsComponent T> i64 GetComponentCount();

  private:
	std::vector<std::unique_ptr<Entity>>				  m_entities;
	std::unordered_map<std::type_index, Query<Component>> m_components;
	EventHandler										 &m_event_handler;
};

template <IsComponent T> i64 Ecs::GetComponentCount() { return m_components[typeid(T)].size(); }

template <IsComponent... Ts> Entity &Ecs::MakeEntity(const std::pair<const char *, Ts *> &...components)
{
	// Using new here because Entities constructor is private and we dont want to
	// friend std::unique_ptr<Entity> to prevent the user from accidently creating
	// illegal entities

	Entity *entity;
	if (!Entity::s_old_ids.empty())
	{
		entity = new Entity(Entity::s_old_ids.top());
		Entity::s_old_ids.pop();
	}
	else
	{
		entity = new Entity(m_entities.size());
	}
	m_entities.push_back(std::unique_ptr<Entity>(entity));

	m_event_handler.PublishEvent(EVENT_TYPE_ENTITY_CREATED, std::make_shared<EntityCreatedEvent>(*entity));

	// Unpack and add components
	(AddComponent(*entity, components.first, components.second), ...);

	return *m_entities.back();
}

template <IsComponent T> void Ecs::AddComponent(Entity &entity, const std::string &name, T *component)
{
	component->SetEntityId(entity.GetId());
	auto shared_component = std::shared_ptr<T>(component);
	entity.AddComponent(name, shared_component);
	m_components[typeid(T)].push_back(shared_component);

	m_event_handler.PublishEvent(EVENT_TYPE_COMPONENT_ADDED, std::make_shared<ComponentAddedEvent>(entity, typeid(T), shared_component));
}

template <IsComponent T> void Ecs::AddComponent(Entity &entity, const std::string &name, std::shared_ptr<T> component)
{
	component->SetEntityId(entity.GetId());
	entity.AddComponent(name, component);
	m_components[typeid(T)].push_back(component);

	m_event_handler.PublishEvent(EVENT_TYPE_COMPONENT_ADDED, std::make_shared<ComponentAddedEvent>(entity, typeid(T), component));
}

template <IsComponent T> void Ecs::RemoveComponents(Entity &entity)
{
	entity.RemoveComponents<T>();
	m_components[typeid(T)].erase(std::remove_if(m_components[typeid(T)].begin(),
												 m_components[typeid(T)].end(),
												 [this, &entity](const std::shared_ptr<Component> &component) { return component->GetEntityId() == entity.GetId(); }),
								  m_components[typeid(T)].end());
}

// This isnt const because we do want to return an empty Query if the component
// doesnt exist (will get automatically inserted by the [] operator for
// std::unordered_map)
template <IsComponent T> Query<T> &Ecs::GetComponents() { return *reinterpret_cast<Query<T> *>(&m_components[typeid(T)]); }
} // namespace Spark

#endif