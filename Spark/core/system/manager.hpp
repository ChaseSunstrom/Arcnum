#ifndef SPARK_MANAGER_HPP
#define SPARK_MANAGER_HPP

#include <core/pch.hpp>
#include "registry.hpp"

namespace Spark
{
	template <typename T>
	class Manager
	{
	public:
		Manager() : m_registry(std::make_unique<Registry<T>>()) {}
		~Manager() = default;

		template <typename... Args>
		T& Create(const std::string& name, Args&&... args)
		{
			T* object = new T(std::forward<Args>(args)...);
			return Register(name, std::unique_ptr<T>(object));
		}

		T& Register(const std::string& name, std::unique_ptr<T> object)
		{
			return m_registry->Register(name, std::move(object));
		}

		T& Get(const std::string& name) const
		{
			return m_registry->Get(name);
		}

		T GetCopy(const std::string& name) const
		{
			return m_registry->GetCopy(name);
		}

		void Remove(const std::string& name)
		{
			m_registry->Remove(name);
		}

		std::vector<std::string> GetKeys() const
		{
			return m_registry->GetKeys();
		}

		void SetRegistry(std::unique_ptr<Registry<T>> registry)
		{
			m_registry = std::move(registry);
		}

		Registry<T>& GetRegistry() const
		{
			return *m_registry;
		}
	private:
		std::unique_ptr<Registry<T>> m_registry;
	};
}

#endif