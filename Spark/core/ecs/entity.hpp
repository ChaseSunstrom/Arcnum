#ifndef SPARK_ENTITY_HPP
#define SPARK_ENTITY_HPP

#include <core/pch.hpp>
#include "query.hpp"

namespace Spark
{
	struct ComponentKey
	{
		std::type_index type;
		std::string name;
		bool operator==(const ComponentKey& other) const = default;
	};
}

template <>
struct std::hash<Spark::ComponentKey>
{
	usize operator()(const Spark::ComponentKey& key) const
	{
		return (std::hash<std::string>()(key.name) ^ (std::hash<std::type_index>()(key.type) << 1) >> 1);
	}
};

namespace Spark
{

	class Entity
	{
	public:
		friend class Ecs;

		operator i64() { return m_id; }

		i64 GetId() const
		{
			return m_id;
		}

		template <IsComponent T>
		T& GetComponent(const std::string& name) const;

		template <IsComponent T>
		bool HasComponent(const std::string& name) const;

		template <IsComponent T>
		Query<T> GetComponents();
	private:
		Entity(i64 id) : m_id(id) {}

		template <IsComponent T>
		void AddComponent(const std::string& name, T* component);

		template <IsComponent T>
		void AddComponent(const std::string& name, std::shared_ptr<T> component);

		template <IsComponent T>
		void RemoveComponent(const std::string& name);

		template <IsComponent T>
		void RemoveComponents();
	private:
		i64	m_id;
		std::unordered_map<ComponentKey, std::shared_ptr<IComponent>> m_components;
	};

	template <IsComponent T>
	T& Entity::GetComponent(const std::string& name) const
	{
		ComponentKey key = { typeid(T), name };
		auto it = m_components.find(key);
		if (it != m_components.end())
		{
			return *static_cast<T*>(it->second.get());
		}
		else
		{
			assert(false && "Component not found");
		}
	}

	template <IsComponent T>
	bool Entity::HasComponent(const std::string& name) const
	{
		ComponentKey key = { typeid(T), name };
		return m_components.find(key) != m_components.end();
	}

	template <IsComponent T>
	Query<T> Entity::GetComponents()
	{
		Query<T> components;
		for (auto& [key, component] : m_components)
		{
			if (key.type == typeid(T))
				components.push_back(component);
		}
		return components;
	}

	template <IsComponent T>
	void Entity::AddComponent(const std::string& name, T* component)
	{
		ComponentKey key = { typeid(T), name };
		m_components[key] = std::shared_ptr<T>(component);
	}

	template <IsComponent T>
	void Entity::AddComponent(const std::string& name, std::shared_ptr<T> component)
	{
		ComponentKey key = { typeid(T), name };
		m_components[key] = component;
	}

	template <IsComponent T>
	void Entity::RemoveComponent(const std::string& name)
	{
		ComponentKey key = { typeid(T), name };
		m_components.erase(key);
	}

	template <IsComponent T>
	void Entity::RemoveComponents()
	{
		std::vector<ComponentKey> keys;
		for (auto& [key, component] : m_components)
		{
			if (key.type == typeid(T))
				keys.push_back(key);
		}

		for (auto& key : keys)
			m_components.erase(key);
	}
}


#endif