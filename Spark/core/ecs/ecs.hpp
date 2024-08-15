#ifndef SPARK_ECS_HPP
#define SPARK_ECS_HPP

#include <core/pch.hpp>
#include "entity.hpp"
#include "component.hpp"

namespace Spark
{
	class Ecs
	{
	public:
		Ecs() = default;
		~Ecs() = default;

		Ecs(const Ecs&) = delete;
		Ecs& operator=(const Ecs&) = delete;

		Entity& MakeEntity();
		Entity&	GetEntity(i64 id) const;
		void RemoveEntity(i64 id);
		
		template <IsComponent T>
		void AddComponent(Entity& entity, const std::string& name, T* component);

		template <IsComponent T>
		Query<T>& GetComponents();
	private:
		std::vector<std::unique_ptr<Entity>> m_entities;
		std::unordered_map<std::type_index, Query<IComponent>> m_components;
	};

	[[deprecated("Use the overload that takes in a std::shared_ptr instead")]]
	template <IsComponent T>
	void Ecs::AddComponent(Entity& entity, const std::string& name, T* component)
	{
		auto shared_component = std::shared_ptr<T>(component);
		entity.AddComponent(name, shared_component);
		m_components[typeid(T)].push_back(shared_component);
	}

	template <IsComponent T>
	void Ecs::AddComponent(Entity& entity, const std::string& name, std::shared_ptr<T> component)
	{
		entity.AddComponent(name, component);
		m_components[typeid(T)].push_back(component);
	}

	// This isnt const because we do want to return an empty Query if the component doesnt exist
	// (will get automatically inserted by the [] operator for std::unordered_map)
	template <IsComponent T>
	Query<T>& Ecs::GetComponents()
	{
		return *reinterpret_cast<Query<T>*>(&m_components[typeid(T)]);
	}
}

#endif
