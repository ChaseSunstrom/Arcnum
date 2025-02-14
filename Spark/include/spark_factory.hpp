#ifndef SPARK_FACTORY_HPP
#define SPARK_FACTORY_HPP

#include "spark_pch.hpp"

namespace spark
{
    class Application;

    using AnyFactoryFn = std::function<std::shared_ptr<void>(Application&)>;

    class FactoryRegistry
    {
    public:
        // Register a factory for type T
        template <typename T>
        void RegisterFactory(std::function<std::shared_ptr<T>(Application&)> factory_fn)
        {
            // Wrap the user’s factory in a type-erased lambda
            AnyFactoryFn wrapper = [factory_fn](Application& app) -> std::shared_ptr<void>
                {
                    return factory_fn(app);
                };
            m_factories[std::type_index(typeid(T))] = wrapper;
        }

        // Retrieve the factory for T, or nullptr if none
        template <typename T>
        std::function<std::shared_ptr<T>(Application&)> GetFactory()
        {
            auto it = m_factories.find(std::type_index(typeid(T)));
            if (it == m_factories.end())
            {
                return nullptr; // no factory
            }
            // convert the AnyFactoryFn -> std::function<std::shared_ptr<T>(Application&)>
            auto& any_fn = it->second;
            return [any_fn](Application& app) -> std::shared_ptr<T>
                {
                    auto ptr_void = any_fn(app);
                    // cast from void to T
                    return std::static_pointer_cast<T>(ptr_void);
                };
        }

    private:
        std::unordered_map<std::type_index, AnyFactoryFn> m_factories;
    };
}


#endif