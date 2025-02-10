#ifndef SPARK_ITEM_HPP
#define SPARK_ITEM_HPP

#include "spark_pch.hpp"
#include <unordered_map>
#include <typeindex>
#include <any>
#include <memory>
#include <stdexcept>
#include <string>

namespace spark
{
    class ItemManager
    {
    public:
        // Adds an item by copying the provided resource and returns a reference to the stored object.
        template <typename T>
        T& AddItem(const std::string& key, const T& res)
        {
            auto ptr = std::make_shared<T>(res);
            T& ref = *ptr;
            // Construct the std::any from a shared_ptr<T>.
            m_items[typeid(T)][key] = std::any(ptr);
            return ref;
        }

        // Adds an item by constructing T in place using perfect forwarding.
        template <typename T, typename... Args>
        T& AddItem(const std::string& key, Args&&... args)
        {
            auto ptr = std::make_shared<T>(std::forward<Args>(args)...);
            T& ref = *ptr;
            m_items[typeid(T)][key] = std::any(ptr);
            return ref;
        }

        // Retrieves a mutable reference to an item. Throws std::runtime_error if not found.
        template <typename T>
        T& GetItem(const std::string& key)
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

        // Retrieves a const reference to an item. Throws std::runtime_error if not found.
        template <typename T>
        const T& GetItem(const std::string& key) const
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

        // Checks whether an item of type T with the specified key exists.
        template <typename T>
        bool HasItem(const std::string& key) const
        {
            auto type_it = m_items.find(typeid(T));
            if (type_it == m_items.end())
            {
                return false;
            }
            const auto& type_map = type_it->second;
            return type_map.find(key) != type_map.end();
        }

        // Removes the item of type T with the specified key.
        template <typename T>
        void RemoveItem(const std::string& key)
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

        // Clears all items from the manager.
        void Clear()
        {
            m_items.clear();
        }

    private:
        std::unordered_map<std::type_index, std::unordered_map<std::string, std::any>> m_items;
    };
}

#endif
