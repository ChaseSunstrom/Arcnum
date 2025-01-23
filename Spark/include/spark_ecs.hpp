#ifndef SPARK_ECS_HPP
#define SPARK_ECS_HPP

#include "spark_pch.hpp"

namespace spark
{
    constexpr usize MAX_COMPONENTS = 64;
    using ComponentSignature = std::bitset<MAX_COMPONENTS>;

    class Coordinator;
    struct IComponentPool;

    class Entity
    {
    public:
        static constexpr u32 INVALID_ID = 0;

        Entity() : m_id(INVALID_ID), m_generation(0) {}
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

    namespace detail
    {
        inline u32 GetUniqueTypeID()
        {
            static std::atomic<u32> last_id{ 0 };
            return last_id++;
        }
    }

    template <typename T>
    u32 GetComponentTypeID() noexcept
    {
        static const u32 type_id = detail::GetUniqueTypeID();
        assert(type_id < MAX_COMPONENTS);
        return type_id;
    }

    template <typename T>
    struct Without { using type = T; };

    template <typename, template <typename> class>
    struct IsSpecialization : std::false_type {};

    template <typename T, template <typename> class Template>
    struct IsSpecialization<Template<T>, Template> : std::true_type {};

    template <typename Tuple>
    struct TupleOfReferences;

    template <typename... Ts>
    struct TupleOfReferences<std::tuple<Ts...>>
    {
        using type = std::tuple<Ts&...>;
    };

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
            m_dense.reserve(1024);
            m_sparse.reserve(1024);
            m_sparse.resize(1024, INVALID_INDEX);
        }

        template <typename... Args>
        T& AddComponent(u32 entity_id, Args&&... args)
        {
            if (HasComponent(entity_id))
            {
                return m_dense[m_sparse[entity_id]].component;
            }

            if (static_cast<usize>(entity_id) >= m_sparse.size())
                m_sparse.resize(static_cast<usize>(entity_id) + 1, INVALID_INDEX);

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
                m_dense[index_to_remove] = std::move(m_dense[last_index]);
                m_sparse[m_dense[index_to_remove].entity_id] = index_to_remove;
            }

            m_dense.pop_back();
            m_sparse[entity_id] = INVALID_INDEX;
        }

        bool HasComponent(u32 entity_id) const override
        {
            return static_cast<usize>(entity_id) < m_sparse.size() && m_sparse[entity_id] != INVALID_INDEX;
        }

        T& GetComponent(u32 entity_id)
        {
            assert(HasComponent(entity_id));
            return m_dense[m_sparse[entity_id]].component;
        }

        const T& GetComponent(u32 entity_id) const
        {
            assert(HasComponent(entity_id));
            return m_dense[m_sparse[entity_id]].component;
        }

        auto Begin() { return m_dense.begin(); }
        auto End() { return m_dense.end(); }
        auto Begin() const { return m_dense.begin(); }
        auto End() const { return m_dense.end(); }

    private:
        struct DenseElement
        {
            u32 entity_id;
            T component;

            template <typename... Args>
            DenseElement(u32 id, Args&&... args) : entity_id(id), component(std::forward<Args>(args)...) {}
        };

        static constexpr usize INVALID_INDEX = static_cast<usize>(-1);
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
            m_active_entities.reserve(1024);
            m_component_pools.resize(MAX_COMPONENTS);
        }

        // Delete copy constructor and copy assignment to prevent copying unique_ptr
        Coordinator(const Coordinator&) = delete;
        Coordinator& operator=(const Coordinator&) = delete;

        template <typename... Components>
        Entity CreateEntity(Components&&... components)
        {
            Entity entity = CreateEntity();
            (AddComponent<Components>(entity, std::forward<Components>(components)), ...);
            return entity;
        }

        Entity CreateEntity()
        {
            if (!m_recycled_entity_ids.empty())
            {
                Entity recycled = m_recycled_entity_ids.top();
                m_recycled_entity_ids.pop();
                u32 id = recycled.GetId();
                m_entity_signatures[id].reset();
                m_entity_generations[id]++;
                m_active_entities[id] = true;
                return Entity(id, m_entity_generations[id]);
            }

            u32 new_id = static_cast<u32>(m_entity_generations.size());
            m_entity_generations.emplace_back(0);
            m_entity_signatures.emplace_back();
            m_active_entities.emplace_back(true);
            return Entity(new_id, 0);
        }

        void DestroyEntity(Entity entity)
        {
            u32 id = entity.GetId();
            if (id == Entity::INVALID_ID || id >= m_entity_generations.size())
                return;

            m_entity_signatures[id].reset();
            m_entity_generations[id]++;
            m_recycled_entity_ids.emplace(Entity(id, m_entity_generations[id]));
            m_active_entities[id] = false;

            for (auto& pool : m_component_pools)
            {
                if (pool && pool->HasComponent(id))
                {
                    pool->RemoveComponent(id);
                }
            }
        }

        template <typename T, typename... Args>
        T& AddComponent(Entity entity, Args&&... args)
        {
            u32 id = entity.GetId();
            u32 type_id = GetComponentTypeID<T>();

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
            u32 type_id = GetComponentTypeID<T>();

            auto pool = GetComponentPool<T>();
            if (pool && pool->HasComponent(id))
            {
                pool->RemoveComponent(id);
                m_entity_signatures[id].set(type_id, false);
            }
        }

        template <typename T>
        T& GetComponent(Entity entity) const
        {
            u32 id = entity.GetId();
            auto pool = GetComponentPool<T>();
            return pool->GetComponent(id);
        }

        template <typename T>
        bool HasComponent(Entity entity) const
        {
            u32 id = entity.GetId();
            if (id >= m_entity_signatures.size())
                return false;
            u32 type_id = GetComponentTypeID<T>();
            return m_entity_signatures[id].test(type_id);
        }

        template <typename... Filters>
        class Query
        {
        public:
            Query(const Coordinator& coordinator)
                : m_coordinator(coordinator)
            {
                ParseFilters<Filters...>();
            }

            template <typename Func>
            void ForEach(Func func) const
            {
                usize entity_count = m_coordinator.m_entity_generations.size();

                for (usize id = 0; id < entity_count; ++id)
                {
                    if (!m_coordinator.m_active_entities[id])
                        continue;

                    const ComponentSignature& signature = m_coordinator.m_entity_signatures[id];

                    if ((signature & m_include_signature) != m_include_signature)
                        continue;

                    if ((signature & m_exclude_signature).any())
                        continue;

                    auto components = GetComponents(static_cast<u32>(id));
                    std::apply([&](auto&... comps) { func(static_cast<u32>(id), comps...); }, components);
                }
            }

        private:
            const Coordinator& m_coordinator;
            ComponentSignature m_include_signature;
            ComponentSignature m_exclude_signature;

            template <typename First, typename... Rest>
            void ParseFilters()
            {
                SetFilter<First>();
                if constexpr (sizeof...(Rest) > 0)
                {
                    ParseFilters<Rest...>();
                }
            }

            void ParseFilters() {}

            template <typename Filter>
            void SetFilter()
            {
                if constexpr (IsSpecialization<Filter, Without>::value)
                {
                    using T = typename Filter::type;
                    m_exclude_signature.set(GetComponentTypeID<T>(), true);
                }
                else
                {
                    m_include_signature.set(GetComponentTypeID<Filter>(), true);
                }
            }

            template <typename... Fs>
            struct ExtractIncluded;

            template <>
            struct ExtractIncluded<>
            {
                using type = std::tuple<>;
            };

            template <typename First, typename... Rest>
            struct ExtractIncluded<First, Rest...>
            {
                using type = std::conditional_t<
                    IsSpecialization<First, Without>::value,
                    typename ExtractIncluded<Rest...>::type,
                    decltype(std::tuple_cat(std::declval<std::tuple<First>>(), std::declval<typename ExtractIncluded<Rest...>::type>()))>;
            };

            using IncludedTypesTuple = typename ExtractIncluded<Filters...>::type;
            using IncludedTypesRefsTuple = typename TupleOfReferences<IncludedTypesTuple>::type;

            IncludedTypesRefsTuple GetComponents(u32 entity_id) const
            {
                return GetComponentsTuple<IncludedTypesTuple>(entity_id, std::make_index_sequence<std::tuple_size<IncludedTypesTuple>::value>{});
            }

            template <typename Tuple, std::size_t... I>
            auto GetComponentsTuple(u32 entity_id, std::index_sequence<I...>) const
                -> std::tuple<std::tuple_element_t<I, Tuple>&...>
            {
                return std::tuple<std::tuple_element_t<I, Tuple>&...>(
                    m_coordinator.GetComponent<std::tuple_element_t<I, Tuple>>(
                        Entity(entity_id, m_coordinator.m_entity_generations[entity_id])
                    )...
                );
            }

        };

        template <typename... Filters>
        Query<Filters...> CreateQuery() const
        {
            return Query<Filters...>(*this);
        }

    private:
        template <typename T>
        void RegisterComponent()
        {
            u32 type_id = GetComponentTypeID<T>();
            if (type_id >= m_component_pools.size())
            {
                m_component_pools.resize(type_id + 1);
            }

            if (!m_component_pools[type_id])
            {
                m_component_pools[type_id] = std::make_unique<ComponentPool<T>>();
            }
        }

        template <typename T>
        ComponentPool<T>* GetComponentPool() const
        {
            u32 type_id = GetComponentTypeID<T>();
            if (type_id >= m_component_pools.size() || !m_component_pools[type_id])
                return nullptr;
            return static_cast<ComponentPool<T>*>(m_component_pools[type_id].get());
        }

        std::stack<Entity> m_recycled_entity_ids;
        std::vector<bool> m_active_entities;
        std::vector<ComponentSignature> m_entity_signatures;
        std::vector<u32> m_entity_generations;
        std::vector<std::unique_ptr<IComponentPool>> m_component_pools;
    };
}

#endif // SPARK_ECS_HPP
