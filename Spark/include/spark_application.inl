#ifndef SPARK_APPLICATION_INL
#define SPARK_APPLICATION_INL

#include "spark_application.hpp"
#include "spark_item.hpp"

namespace spark
{

	// Adds an item by copying the provided resource.
	template <typename T>
	T& Application::AddItem(const std::string& key, const T& res)
	{
		return m_item_manager->AddItem(key, res);
	}

	// Adds an item by constructing T in place using perfect forwarding.
	template <typename T, typename... Args>
	T& Application::AddItem(const std::string& key, Args&&... args)
	{
		return m_item_manager->AddItem<T>(key, std::forward<Args>(args)...);
	}

	// Retrieves a mutable reference to an item. Throws std::runtime_error if not found.
	template <typename T>
	T& Application::GetItem(const std::string& key)
	{
		return m_item_manager->GetItem<T>(key);
	}

	// Retrieves a const reference to an item. Throws std::runtime_error if not found.
	template <typename T>
	const T& Application::GetItem(const std::string& key) const
	{
		return m_item_manager->GetItem<T>(key);
	}

	// Checks whether an item of type T with the specified key exists.
	template <typename T>
	bool Application::HasItem(const std::string& key) const
	{
		return m_item_manager->HasItem<T>(key);
	}

	template <typename T>
	void Application::RemoveItem(const std::string& key)
	{
		m_item_manager->RemoveItem<T>(key);
	}

}

#endif