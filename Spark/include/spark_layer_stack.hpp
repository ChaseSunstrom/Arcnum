#ifndef SPARK_LAYER_STACK_HPP
#define SPARK_LAYER_STACK_HPP

#include "spark_pch.hpp"
#include "spark_layer.hpp"

#include <unordered_map>
#include <typeindex>
#include <memory>

namespace spark
{
    class LayerStack
    {
    public:
        LayerStack() = default;
        ~LayerStack() = default;

        // PushLayer inserts the layer by its type.
        template <typename T, typename... Args>
        T* PushLayer(Args&&... args)
        {
            static_assert(std::is_base_of_v<ILayer, T>, "T must derive from ILayer");
            std::type_index key{ typeid(T) };

            // You might want to check if a layer of this type already exists.
            // If not, create and attach the layer.
            auto layer = std::make_unique<T>(std::forward<Args>(args)...);
            layer->OnAttach();
            m_layers.emplace(key, std::move(layer));
            return GetLayer<T>();
        }

        // RemoveLayer removes and detaches a layer by type.
        template <typename T>
        void RemoveLayer()
        {
            std::type_index key{ typeid(T) };
            auto it = m_layers.find(key);
            if (it != m_layers.end())
            {
                it->second->OnDetach();
                m_layers.erase(it);
            }
        }

        // GetLayer returns a pointer to the layer stored under type T, or nullptr if not found.
        template <typename T>
        T* GetLayer()
        {
            std::type_index key{ typeid(T) };
            auto it = m_layers.find(key);
            if (it != m_layers.end())
            {
                return dynamic_cast<T*>(it->second.get());
            }
            return nullptr;
        }

        // If needed, a const version of GetLayer.
        template <typename T>
        const T* GetLayer() const
        {
            std::type_index key{ typeid(T) };
            auto it = m_layers.find(key);
            if (it != m_layers.end())
            {
                return dynamic_cast<const T*>(it->second.get());
            }
            return nullptr;
        }

        // Remove all layers (calls OnDetach on each).
        void Clear()
        {
            for (auto&  layer : m_layers | std::views::values)
            {
                layer->OnDetach();
            }
            m_layers.clear();
        }

        void Start()
        {
            for (auto& layer : m_layers | std::views::values)
            {
                layer->OnStart();
            }
        }

        void Stop()
        {
            for (auto& layer : m_layers | std::views::values)
            {
                layer->OnDetach();
            }
        }

        void Update(f32 dt)
        {
            for (auto& layer : m_layers | std::views::values)
            {
                layer->OnUpdate(dt);
            }
        }

    private:
        std::unordered_map<std::type_index, std::unique_ptr<ILayer>> m_layers;
    };
}

#endif
