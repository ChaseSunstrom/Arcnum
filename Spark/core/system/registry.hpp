#ifndef SPARK_REGISTRY_HPP
#define SPARK_REGISTRY_HPP

#include <core/pch.hpp>

namespace Spark
{
	// Basically just a wrapper around a std::unordered_map<std::string, std::unique_ptr<T>>
	// so we can name the elements put into this (very useful for keeping track of meshes, materials, shaders, etc.)
	template <typename T>
	class Registry
	{
	public:
		Registry() = default;
		~Registry() = default;

		// We do NOT want to be able to copy large amounts of objects.
		Registry(const Registry&) = delete;
		Registry& operator=(const Registry&) = delete;

		template <typename... Args>
		T& Register(const std::string& name, Args&&... args)
		{
			// We might not be able to use the make_unique constructor
			// because it may be private, such as the case with GLMesh
			T* ptr = new T(std::forward<Args>(args)...);
			T& ref = *ptr;
			m_registry[name] = std::unique_ptr<T>(ptr);
			return ref;
		}

		T& Register(const std::string& name, std::unique_ptr<T> object)
		{
			T& ref = *object;
			m_registry[name] = std::move(object);
			return ref;
		}

		T* Get(const std::string& name)
		{
			auto it = m_registry.find(name);
			if (it != m_registry.end())
				return it->second.get();
			return nullptr;
		}

		void Remove(const std::string& name)
		{
			auto it = m_registry.find(name);
			if (it != m_registry.end())
				m_registry.erase(it);
		}

		std::vector<std::string> GetKeys() const
		{
			std::vector<std::string> keys;
			for (auto& [key, value] : m_registry)
				keys.push_back(key);
			return keys;
		}

		std::vector<T*> GetValues() const
		{
			std::vector<T*> values;
			for (auto& [key, value] : m_registry)
				values.push_back(value.get());
			return values;
		}

		bool Contains(const std::string& name) const
		{
			return m_registry.find(name) != m_registry.end();
		}

		void Clear()
		{
			m_registry.clear();
		}

		u32 Size() const
		{
			return m_registry.size();
		}
	private:
		std::unordered_map<std::string, std::unique_ptr<T>> m_registry;
	};
}

#endif