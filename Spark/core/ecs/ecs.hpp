#ifndef SPARK_ECS_HPP
#define SPARK_ECS_HPP

#include "component.hpp"
#include "entity.hpp"
#include "system.hpp"
#include <core/event/event.hpp>
#include <core/event/event_handler.hpp>
#include <core/pch.hpp>

namespace Spark {
class Ecs {
  public:
	Ecs(EventHandler& event_handler)
		: m_event_handler(event_handler) {}
	~Ecs()                     = default;

	Ecs(const Ecs&)            = delete;
	Ecs& operator=(const Ecs&) = delete;

	void Start();
	void Update(f32 delta_time);
	void Shutdown();

	template <IsComponent... Ts>
	Entity& MakeEntity(const std::pair<const char*, Ts>&... components);
	Entity& GetEntity(const u32 id) const;
	i64 GetEntityCount() const;
	void DestroyEntity(const u32 id);

	template <IsComponent T>
	void AddComponent(Entity& entity, const std::string& name, const T& component);

	template <IsSystem T>
	void AddSystem(std::unique_ptr<T> system);

	template <IsComponent T>
	void RemoveComponents(Entity& entity);

	void RemoveAllEntityComponents(Entity& entity);
	void RemoveAllEntityComponents(const u32 id);

	template <IsComponent T>
	std::vector<T>& GetComponents();

	template <IsComponent T>
	i64 GetComponentCount();

	template <IsComponent T>
	T& GetComponent(Entity& entity, const std::string& name);

	template <IsComponent T>
	T& GetComponent(const u32 id, const std::string& name);

  private:
	template <IsComponent T>
	ComponentArray<T>& GetComponentArray();

  private:
	std::vector<Entity> m_entities;
	std::vector<std::unique_ptr<System>> m_systems;
	std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> m_components;
	EventHandler& m_event_handler;
};

template <IsComponent T>
T& Ecs::GetComponent(Entity& entity, const std::string& name) {
	auto& component_array = GetComponentArray<T>();
	return component_array.GetData(entity.GetId());
}

template <IsComponent T>
T& Ecs::GetComponent(const u32 id, const std::string& name) {
	auto& component_array = GetComponentArray<T>();
	return component_array.GetData(id);
}

template <IsComponent T>
ComponentArray<T>& Ecs::GetComponentArray() {
	auto& ptr = m_components[typeid(T)];
	if (!ptr) {
		ptr = std::make_unique<ComponentArray<T>>();
	}
	return *static_cast<ComponentArray<T>*>(ptr.get());
}

template <IsComponent T>
i64 Ecs::GetComponentCount() {
	return GetComponentArray<T>().GetAllComponents().size();
}

template <IsComponent... Ts>
Entity& Ecs::MakeEntity(const std::pair<const char*, Ts>&... components) {
	u32 id = m_entities.empty() ? 0 : (m_entities.back().GetId() + 1);
	Entity entity(id);
	m_entities.push_back(entity);

	m_event_handler.PublishEvent<EntityCreatedEvent>(std::make_shared<EntityCreatedEvent>(entity));

	(AddComponent(entity, components.first, components.second), ...);

	return entity;
}

template <IsComponent T>
void Ecs::AddComponent(Entity& entity, const std::string& name, const T& component) {
	auto& component_array = GetComponentArray<T>();
	component_array.InsertData(entity.GetId(), component);
	entity.AddComponent<T>(name);
	m_event_handler.PublishEvent<ComponentAddedEvent<T>>(
		std::make_shared<ComponentAddedEvent<T>>(entity, ComponentEventType::ADDED, component));
}

template <IsSystem T>
void Ecs::AddSystem(std::unique_ptr<T> system) {
	m_systems.push_back(std::move(system));
}

template <IsComponent T>
void Ecs::RemoveComponents(Entity& entity) {
	auto& component_array = GetComponentArray<T>();
	component_array.RemoveEntity(entity.GetId());
	entity.RemoveComponents<T>();
}

template <IsComponent T>
Query<T>& Ecs::GetComponents() {
	return GetComponentArray<T>().GetAllComponents();
}
} // namespace Spark

#endif