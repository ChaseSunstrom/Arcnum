#ifndef SPARK_ECS_HPP
#define SPARK_ECS_HPP

#include "spark_pch.hpp"

namespace spark
{
    constexpr usize MAX_COMPONENTS = 64;
    using ComponentSignature = std::bitset<MAX_COMPONENTS>;

    class Entity
    {
    public:
        static constexpr u32 InvalidId = 0;

        Entity() : m_id(InvalidId), m_generation(0) {}
        Entity(u32 id, u32 generation) : m_id(id), m_generation(generation) {}

        u32 GetId() const { return m_id; }
        u32 GetGeneration() const { return m_generation; }

        bool operator==(const Entity& other) const
        {
            return m_id == other.m_id && m_generation == other.m_generation;
        }

        bool operator!=(const Entity& other) const
        {
            return !(*this == other);
        }

    private:
        u32 m_id;
        u32 m_generation;
    };

    // TypeID Generator
    namespace detail
    {
        inline u32 GetUniqueTypeID()
        {
            static std::atomic<u32> last_id{ 0 };
            return last_id++;
        }
    }

    template <typename T>
    inline u32 GetComponentTypeID() noexcept
    {
        static const u32 type_id = detail::GetUniqueTypeID();
        assert(type_id < MAX_COMPONENTS && "Exceeded maximum number of components");
        return type_id;
    }

    struct IComponentPool
    {
        virtual ~IComponentPool() = default;
        virtual void RemoveComponent(u32 entity_id) = 0;
        virtual bool HasComponent(u32 entity_id) const = 0;
    };

    template <typename T>
    class ComponentPool : public IComponentPool
    {
    public:
        ComponentPool()
        {
            m_sparse.reserve(1024); // Preallocate
        }

        ~ComponentPool() override = default;

        // Add component using entity ID
        template <typename... Args>
        T& AddComponent(u32 entity_id, Args&&... args)
        {
            if (HasComponent(entity_id))
            {
                Logger::Logln(LogLevel::WARN, "Entity %u already has component: %s", entity_id, typeid(T).name());
                return m_dense[m_sparse[entity_id]].component;
            }

            if (entity_id >= m_sparse.size())
                m_sparse.resize(entity_id + 1, InvalidIndex);

            m_sparse[entity_id] = m_dense.size();
            m_dense.emplace_back(entity_id, std::forward<Args>(args)...);
            return m_dense.back().component;
        }

        void RemoveComponent(u32 entity_id) override
        {
            if (!HasComponent(entity_id))
                return;

            usize index_to_remove = m_sparse[entity_id];
            usize last_index = m_dense.size() - 1;

            if (index_to_remove != last_index)
            {
                // Swap with last
                m_dense[index_to_remove] = std::move(m_dense[last_index]);
                m_sparse[m_dense[index_to_remove].entity_id] = index_to_remove;
            }

            m_dense.pop_back();
            m_sparse[entity_id] = InvalidIndex;
        }

        bool HasComponent(u32 entity_id) const override
        {
            return entity_id < m_sparse.size() && m_sparse[entity_id] != InvalidIndex;
        }

        T& GetComponent(u32 entity_id)
        {
            assert(HasComponent(entity_id) && "Component not found");
            return m_dense[m_sparse[entity_id]].component;
        }

        const T& GetComponent(u32 entity_id) const
        {
            assert(HasComponent(entity_id) && "Component not found");
            return m_dense[m_sparse[entity_id]].component;
        }

        // Iterators for component access
        auto begin() { return m_dense.begin(); }
        auto end() { return m_dense.end(); }
        auto begin() const { return m_dense.begin(); }
        auto end() const { return m_dense.end(); }

    private:
        struct DenseElement
        {
            u32 entity_id;
            T component;

            DenseElement(u32 id, T&& comp) : entity_id(id), component(std::move(comp)) {}
            template <typename... Args>
            DenseElement(u32 id, Args&&... args) : entity_id(id), component(std::forward<Args>(args)...) {}
        };

        static constexpr usize InvalidIndex = static_cast<usize>(-1);
        std::vector<DenseElement> m_dense;
        std::vector<usize> m_sparse;
    };

    class Coordinator
    {
    public:
        Coordinator()
        {
            m_entity_signatures.reserve(1024);
            m_entity_generations.reserve(1024);
        }

        ~Coordinator()
        {
            // Clean up component pools
            for (auto& pool : m_component_pools)
            {
                delete pool;
            }
        }

        // Entity Management
        Entity CreateEntity()
        {
            if (!m_recycled_entity_ids.empty())
            {
                Entity recycled = m_recycled_entity_ids.top();
                m_recycled_entity_ids.pop();
                m_entity_signatures[recycled.GetId()] = ComponentSignature();
                return recycled;
            }

            u32 new_id = static_cast<u32>(m_entity_generations.size());
            m_entity_generations.push_back(0);
            m_entity_signatures.emplace_back();
            return Entity(new_id, 0);
        }

        void DestroyEntity(Entity entity)
        {
            u32 id = entity.GetId();
            if (id == Entity::InvalidId)
                return;

            m_entity_signatures[id].reset();

            m_entity_generations[id]++;
            m_recycled_entity_ids.emplace(id, m_entity_generations[id]);

            for (usize i = 0; i < m_component_pools.size(); ++i)
            {
                if (m_component_pools[i])
                {
                    m_component_pools[i]->RemoveComponent(id);
                }
            }
        }

        template <typename T, typename... Args>
        T& AddComponent(Entity entity, Args&&... args)
        {
            u32 id = entity.GetId();
            auto type_id = GetComponentTypeID<T>();

            RegisterComponent<T>();

            auto pool = GetComponentPool<T>();
            T& component = pool->AddComponent(id, std::forward<Args>(args)...);

            m_entity_signatures[id].set(type_id, true);

            return component;
        }

        template <typename T>
        void RemoveComponent(Entity entity)
        {
            u32 id = entity.GetId();
            auto type_id = GetComponentTypeID<T>();

            auto pool = GetComponentPool<T>();
            if (pool && pool->HasComponent(id))
            {
                pool->RemoveComponent(id);
                m_entity_signatures[id].set(type_id, false);
            }
        }

        template <typename T>
        T& GetComponent(Entity entity)
        {
            u32 id = entity.GetId();
            auto pool = GetComponentPool<T>();
            return pool->GetComponent(id);
        }

        template <typename T>
        bool HasComponent(Entity entity) const
        {
            u32 id = entity.GetId();
            auto type_id = GetComponentTypeID<T>();
            return m_entity_signatures[id].test(type_id);
        }

        template <typename... Components>
        std::vector<Entity> QueryEntities()
        {
            ComponentSignature required_signature;
            (required_signature.set(GetComponentTypeID<Components>()), ...);

            std::vector<Entity> result;
            usize entity_count = static_cast<usize>(m_entity_generations.size());

            // Iterate through all entities and match signatures
            for (usize id = 0; id < entity_count; ++id)
            {
                // Skip invalid entities
                if (id == Entity::InvalidId)
                    continue;

                if ((m_entity_signatures[id] & required_signature) == required_signature)
                {
                    if (m_recycled_entity_ids.empty() || m_recycled_entity_ids.top().GetId() != id)
                    {
                        result.emplace_back(id, m_entity_generations[id]);
                    }
                }
            }

            return result;
        }

        // Optimized Query with Archetypes (Optional Extension)
        // Further optimizations can be implemented here

    private:
        template <typename T>
        void RegisterComponent()
        {
            auto type_id = GetComponentTypeID<T>();
            if (type_id >= m_component_pools.size())
            {
                m_component_pools.resize(type_id + 1, nullptr);
            }

            if (!m_component_pools[type_id])
            {
                m_component_pools[type_id] = new ComponentPool<T>();
            }
        }

        template <typename T>
        ComponentPool<T>* GetComponentPool() const
        {
            auto type_id = GetComponentTypeID<T>();
            if (type_id >= m_component_pools.size() || !m_component_pools[type_id])
                return nullptr;
            return static_cast<ComponentPool<T>*>(m_component_pools[type_id]);
        }

        // Entity Recycling with Generation Counts
        std::stack<Entity> m_recycled_entity_ids;

        // Entity Signatures
        std::vector<ComponentSignature> m_entity_signatures;
        std::vector<u32> m_entity_generations;

        // Component Pools
        std::vector<IComponentPool*> m_component_pools;
    };
}

#endif // SPARK_ECS_HPP
