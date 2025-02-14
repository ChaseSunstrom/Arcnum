#ifndef SPARK_ITEM_INL
#define SPARK_ITEM_INL


// Now we include the full definition of Application
#include "spark_application.hpp"

namespace spark
{
    // Constructor
    inline ItemManager::ItemManager(Application* app)
        : m_app(app)
    {
    }

    // Adds an item by copying the provided resource
    // For adding an item by copying the provided resource.
    template <typename T>
    inline T& ItemManager::AddItem(const std::string& key, const T& res)
    {
        if (HasItem<T>(key))
        {
            return GetItem<T>(key);
        }

        auto factory_fn = m_app->GetFactoryRegistry().template GetFactory<T>();

        if constexpr (std::is_abstract_v<T>)
        {
            // For abstract types, we must have a factory.
            if (!factory_fn)
                throw std::runtime_error("No factory registered for abstract type");
            auto ptr = factory_fn(*m_app);
            T& ref = *ptr;
            m_items[std::type_index(typeid(T))][key] = std::any(ptr);
            return ref;
        }
        else
        {
            if (factory_fn)
            {
                auto ptr = factory_fn(*m_app);
                T& ref = *ptr;
                m_items[std::type_index(typeid(T))][key] = std::any(ptr);
                return ref;
            }
            else
            {
                auto ptr = std::make_shared<T>(res);
                T& ref = *ptr;
                m_items[std::type_index(typeid(T))][key] = std::any(ptr);
                return ref;
            }
        }
    }

    // For adding an item by constructing T in place with perfect forwarding.
    template <typename T, typename... Args>
    inline T& ItemManager::AddItem(const std::string& key, Args&&... args)
    {
        if (HasItem<T>(key))
        {
            return GetItem<T>(key);
        }

        auto factory_fn = m_app->GetFactoryRegistry().template GetFactory<T>();

        if constexpr (std::is_abstract_v<T>)
        {
            // For abstract types, require a factory.
            if (!factory_fn)
                throw std::runtime_error("No factory registered for abstract type");
            auto ptr = factory_fn(*m_app);
            T& ref = *ptr;
            m_items[std::type_index(typeid(T))][key] = std::any(ptr);
            return ref;
        }
        else
        {
            if (factory_fn)
            {
                auto ptr = factory_fn(*m_app);
                T& ref = *ptr;
                m_items[std::type_index(typeid(T))][key] = std::any(ptr);
                return ref;
            }
            else
            {
                auto ptr = std::make_shared<T>(std::forward<Args>(args)...);
                T& ref = *ptr;
                m_items[std::type_index(typeid(T))][key] = std::any(ptr);
                return ref;
            }
        }
    }

    // GetItem (mutable)
    template <typename T>
    inline T& ItemManager::GetItem(const std::string& key)
    {
        auto type_it = m_items.find(typeid(T));
        if (type_it == m_items.end())
        {
            throw std::runtime_error("No items found for requested type: " + std::string(typeid(T).name()));
        }

        auto& type_map = type_it->second;
        auto item_it = type_map.find(key);
        if (item_it == type_map.end())
        {
            throw std::runtime_error("Item with key \"" + key + "\" not found for type: " + std::string(typeid(T).name()));
        }

        return *std::any_cast<std::shared_ptr<T>&>(item_it->second);
    }

    // GetItem (const)
    template <typename T>
    inline const T& ItemManager::GetItem(const std::string& key) const
    {
        auto type_it = m_items.find(typeid(T));
        if (type_it == m_items.end())
        {
            throw std::runtime_error("No items found for requested type: " + std::string(typeid(T).name()));
        }

        const auto& type_map = type_it->second;
        auto item_it = type_map.find(key);
        if (item_it == type_map.end())
        {
            throw std::runtime_error("Item with key \"" + key + "\" not found for type: " + std::string(typeid(T).name()));
        }

        return *std::any_cast<const std::shared_ptr<T>&>(item_it->second);
    }

    // HasItem
    template <typename T>
    inline bool ItemManager::HasItem(const std::string& key) const
    {
        auto type_it = m_items.find(typeid(T));
        if (type_it == m_items.end())
        {
            return false;
        }
        const auto& type_map = type_it->second;
        return (type_map.find(key) != type_map.end());
    }

    // RemoveItem
    template <typename T>
    inline void ItemManager::RemoveItem(const std::string& key)
    {
        auto type_it = m_items.find(typeid(T));
        if (type_it != m_items.end())
        {
            auto& type_map = type_it->second;
            auto item_it = type_map.find(key);
            if (item_it != type_map.end())
            {
                type_map.erase(item_it);
                if (type_map.empty())
                {
                    m_items.erase(type_it);
                }
            }
        }
    }

    inline void ItemManager::Clear()
    {
        m_items.clear();
    }
}

#endif