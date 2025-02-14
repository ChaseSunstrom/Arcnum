#ifndef SPARK_ITEM_HPP
#define SPARK_ITEM_HPP

#include "spark_pch.hpp"

namespace spark
{
    class Application;


    class SPARK_API ItemManager
    {
    public:
        // Construct with a pointer or reference to the Application
        // (we'll store just a pointer to avoid storing incomplete type by value)
        explicit ItemManager(Application* app);

        // Declaration of template "AddItem" methods (no inline definitions here!)
        template <typename T>
        T& AddItem(const std::string& key, const T& res);

        template <typename T, typename... Args>
        T& AddItem(const std::string& key, Args&&... args);

        template <typename T>
        T& GetItem(const std::string& key);

        template <typename T>
        const T& GetItem(const std::string& key) const;

        template <typename T>
        bool HasItem(const std::string& key) const;

        template <typename T>
        void RemoveItem(const std::string& key);

        void Clear();

    private:
        Application* m_app;

        // typeid(T) => { key => std::any(std::shared_ptr<T>) }
        std::unordered_map<std::type_index, std::unordered_map<std::string, std::any>> m_items;
    };
}

#include "spark_item.inl"

#endif