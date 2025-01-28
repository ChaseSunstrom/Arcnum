#ifndef SPARK_ECS_HPP
#define SPARK_ECS_HPP

#include "spark_pch.hpp"

namespace spark
{
    constexpr usize MAX_COMPONENTS = 64;
    using ComponentSignature = std::bitset<MAX_COMPONENTS>;

    // The chunk size in bytes (for SoA storage inside archetypes)
    constexpr usize CHUNK_SIZE_BYTES = 16 * 1024;

    class Entity
    {
    public:
        static constexpr u32 INVALID_ID = 0;

        Entity()
            : m_id(INVALID_ID)
            , m_generation(0)
        {
        }

        Entity(u32 id, u32 generation)
            : m_id(id)
            , m_generation(generation)
        {
        }

        u32 GetId() const
        {
            return m_id;
        }

        u32 GetGeneration() const
        {
            return m_generation;
        }

        bool operator==(const Entity& other) const
        {
            return (m_id == other.m_id) && (m_generation == other.m_generation);
        }

        bool operator!=(const Entity& other) const
        {
            return !(*this == other);
        }

    private:
        u32 m_id;
        u32 m_generation;
    };

    // ------------------------------------------------------------------------
    // GetComponentTypeID
    // ------------------------------------------------------------------------
    namespace detail
    {
        inline u32 GetUniqueTypeID()
        {
            static std::atomic<u32> s_last_id{ 0 };
            return s_last_id++;
        }
    }

    template <typename T>
    u32 GetComponentTypeID() noexcept
    {
        static const u32 s_type_id = detail::GetUniqueTypeID();
        // Ensure we don't exceed MAX_COMPONENTS
        assert(s_type_id < MAX_COMPONENTS);
        return s_type_id;
    }

    template <typename T>
    struct Without
    {
        using type = T;
    };

    template <typename, template <typename> class>
    struct IsSpecialization : std::false_type
    {
    };

    template <typename T, template <typename> class Template>
    struct IsSpecialization<Template<T>, Template> : std::true_type
    {
    };

    template <typename Tuple>
    struct TupleOfReferences;

    template <typename... Ts>
    struct TupleOfReferences<std::tuple<Ts...>>
    {
        using type = std::tuple<Ts&...>;
    };

    using CopyFn = void (*)(const void* src, void* dst);

    template <typename T>
    void CopyComponentFn(const void* s, void* d)
    {
        const T* src = static_cast<const T*>(s);
        T* dst = static_cast<T*>(d);
        *dst = *src;
    }

    // Global arrays for all component types:
    static inline CopyFn g_copy_fns[MAX_COMPONENTS] = { nullptr };
    static inline usize g_type_sizes[MAX_COMPONENTS] = { 0 };

    template <typename T>
    void InitTypeIfNeeded()
    {
        u32 tid = GetComponentTypeID<T>();
        if (g_type_sizes[tid] == 0)
        {
            g_type_sizes[tid] = sizeof(T);
            if (!g_copy_fns[tid])
            {
                g_copy_fns[tid] = &CopyComponentFn<T>;
            }
        }
    }

    // ------------------------------------------------------------------------
    // Chunk: SoA data portion for a given signature
    // ------------------------------------------------------------------------
    class Chunk
    {
    public:
        Chunk(const ComponentSignature& signature, usize chunk_size_bytes = CHUNK_SIZE_BYTES)
            : m_signature(signature)
            , m_capacity_bytes(chunk_size_bytes)
        {
            // Initialize the type map to -1 for fast "not found" checks
            for (int i = 0; i < MAX_COMPONENTS; ++i)
            {
                m_type_map[i] = -1;
            }
        }

        void InitLayout()
        {
            m_type_ids.clear();
            m_type_sizes.clear();

            // Sum up the sizes of each active bit
            usize total_size_per_entity = 0;
            for (u32 tid = 0; tid < MAX_COMPONENTS; ++tid)
            {
                if (m_signature.test(tid))
                {
                    usize sz = g_type_sizes[tid];
                    if (sz == 0)
                    {
                        // Could log warning or handle error
                    }
                    m_type_ids.push_back(tid);
                    m_type_sizes.push_back(sz);
                    total_size_per_entity += sz;
                }
            }

            if (total_size_per_entity == 0)
            {
                // empty signature => can hold "arbitrary" number
                m_capacity_entities = m_capacity_bytes;
            }
            else
            {
                m_capacity_entities = m_capacity_bytes / total_size_per_entity;
            }

            m_entities.resize(m_capacity_entities);
            m_entity_count = 0;

            // For each type, allocate a SoA array
            m_component_arrays.resize(m_type_ids.size());
            for (usize i = 0; i < m_type_ids.size(); ++i)
            {
                usize needed = m_capacity_entities * m_type_sizes[i];
                m_component_arrays[i].resize(needed);

                // Fill the map so we can do O(1) lookups for each tid
                u32 tid = m_type_ids[i];
                m_type_map[tid] = static_cast<int>(i);
            }
        }

        bool HasSpace() const
        {
            return (m_entity_count < m_capacity_entities);
        }

        usize Size() const
        {
            return m_entity_count;
        }

        Entity GetEntity(usize i) const
        {
            return m_entities[i];
        }

        usize AddEntity(Entity e)
        {
            usize idx = m_entity_count;
            m_entity_count++;
            m_entities[idx] = e;
            return idx;
        }

        void RemoveEntity(usize idx)
        {
            usize last_idx = m_entity_count - 1;
            if (idx != last_idx)
            {
                m_entities[idx] = m_entities[last_idx];
                // For each active type, do swap
                for (usize i = 0; i < m_type_ids.size(); ++i)
                {
                    SwapData(i, idx, last_idx);
                }
            }
            m_entity_count--;
        }

        void SwapData(usize arr_index, usize idx_a, usize idx_b)
        {
            if (idx_a == idx_b)
            {
                return;
            }
            usize sz = m_type_sizes[arr_index];
            std::byte* arr = m_component_arrays[arr_index].data();
            std::byte* A = arr + (idx_a * sz);
            std::byte* B = arr + (idx_b * sz);
            for (usize i = 0; i < sz; ++i)
            {
                std::byte tmp = A[i];
                A[i] = B[i];
                B[i] = tmp;
            }
        }

        void CopyTo(usize idx_src, Chunk* dst_chunk, usize idx_dst) const
        {
            // We assume same signature => same m_type_ids ordering
            for (usize i = 0; i < m_type_ids.size(); ++i)
            {
                u32 tid = m_type_ids[i];
                CopyFn fn = g_copy_fns[tid];
                usize sz = m_type_sizes[i];

                const void* s_ptr = &m_component_arrays[i][idx_src * sz];
                void* d_ptr = &dst_chunk->m_component_arrays[i][idx_dst * sz];

                if (fn)
                {
                    fn(s_ptr, d_ptr);
                }
                else
                {
                    std::memcpy(d_ptr, s_ptr, sz);
                }
            }
        }

        void* GetComponentData(u32 tid, usize idx)
        {
            int arr_index = m_type_map[tid];
            if (arr_index < 0)
            {
                return nullptr; // not found
            }
            usize sz = m_type_sizes[arr_index];
            return &m_component_arrays[arr_index][idx * sz];
        }

        const void* GetComponentData(u32 tid, usize idx) const
        {
            int arr_index = m_type_map[tid];
            if (arr_index < 0)
            {
                return nullptr;
            }
            usize sz = m_type_sizes[arr_index];
            return &m_component_arrays[arr_index][idx * sz];
        }

        const ComponentSignature& GetSignature() const
        {
            return m_signature;
        }

        const std::vector<Entity>& GetEntities() const
        {
            return m_entities;
        }

    private:
        ComponentSignature m_signature;
        usize m_capacity_bytes = 0;
        usize m_capacity_entities = 0;
        usize m_entity_count = 0;

        // SoA arrays
        std::vector<u32> m_type_ids;  // which TIDs are in this chunk
        std::vector<usize> m_type_sizes;
        std::vector<std::vector<std::byte>> m_component_arrays;
        std::vector<Entity> m_entities;

        // Fast lookup: tid -> index in m_component_arrays (or -1 if not present)
        int m_type_map[MAX_COMPONENTS];
    };

    class Archetype
    {
    public:
        Archetype(const ComponentSignature& sig)
            : m_signature(sig)
        {
            AddChunk();
        }

        const ComponentSignature& GetSignature() const
        {
            return m_signature;
        }

        std::pair<Chunk*, usize> AddEntity(Entity e)
        {
            Chunk& last = m_chunks.back();
            if (!last.HasSpace())
            {
                AddChunk();
            }
            Chunk& chunk_ref = m_chunks.back();
            usize idx = chunk_ref.AddEntity(e);
            return { &chunk_ref, idx };
        }

        void RemoveEntity(Chunk* chunk_ptr, usize idx_in_chunk)
        {
            chunk_ptr->RemoveEntity(idx_in_chunk);
        }

        void CopyEntity(Chunk* from_chunk, usize idxA, Chunk* to_chunk, usize idxB)
        {
            from_chunk->CopyTo(idxA, to_chunk, idxB);
        }

        std::vector<Chunk>& GetChunks()
        {
            return m_chunks;
        }

        const std::vector<Chunk>& GetChunks() const
        {
            return m_chunks;
        }

    private:
        void AddChunk()
        {
            m_chunks.emplace_back(m_signature);
            m_chunks.back().InitLayout();
        }

    private:
        ComponentSignature m_signature;
        std::vector<Chunk> m_chunks;
    };

    struct EntityLocation
    {
        Archetype* archetype_ptr = nullptr;
        Chunk* chunk_ptr = nullptr;
        usize index_in_chunk = 0;
    };

    class Coordinator
    {
    public:
        Coordinator()
        {
            m_active_entities.reserve(1024);
            m_entity_generations.reserve(1024);
        }

        Coordinator(const Coordinator&) = delete;
        Coordinator& operator=(const Coordinator&) = delete;

        // Create entity with optional components
        template <typename... Components>
        Entity CreateEntity(Components&&... comps)
        {
            (InitTypeIfNeeded<std::remove_reference_t<Components>>(), ...);

            Entity e = CreateEmptyEntity();

            if constexpr (sizeof...(comps) > 0)
            {
                ComponentSignature sig;
                (sig.set(GetComponentTypeID<std::remove_reference_t<Components>>()), ...);

                Archetype* arch = GetOrCreateArchetype(sig);
                auto [chunk_ptr, idx] = arch->AddEntity(e);

                FillComponents<Components...>(chunk_ptr, idx, std::forward<Components>(comps)...);
                SetEntityLocation(e.GetId(), { arch, chunk_ptr, idx });
            }
            else
            {
                // empty signature
                ComponentSignature sig;
                Archetype* arch = GetOrCreateArchetype(sig);
                auto [chunk_ptr, idx] = arch->AddEntity(e);

                SetEntityLocation(e.GetId(), { arch, chunk_ptr, idx });
            }

            return e;
        }

        void DestroyEntity(Entity e)
        {
            u32 id = e.GetId();
            if (id == Entity::INVALID_ID || id >= m_entity_generations.size())
            {
                return;
            }

            // Invalidate entity, recycle ID
            m_entity_generations[id]++;
            m_active_entities[id] = false;
            m_recycled_ids.push(e);

            if (id < m_entity_locations.size())
            {
                EntityLocation& loc = m_entity_locations[id];
                if (loc.archetype_ptr)
                {
                    loc.archetype_ptr->RemoveEntity(loc.chunk_ptr, loc.index_in_chunk);
                    loc.archetype_ptr = nullptr;
                }
            }
        }

        // Add a component T
        template <typename T, typename... Args>
        T& AddComponent(Entity e, Args&&... args)
        {
            InitTypeIfNeeded<T>();

            u32 id = e.GetId();
            if (id >= m_entity_locations.size())
            {
                // This shouldn't happen unless the Entity is invalid or we've not resized properly
                throw std::runtime_error("Invalid entity ID in AddComponent.");
            }
            EntityLocation old_loc = m_entity_locations[id];
            Archetype* old_arch = old_loc.archetype_ptr;
            ComponentSignature old_sig = old_arch->GetSignature();

            u32 tid = GetComponentTypeID<T>();
            if (old_sig.test(tid))
            {
                // Already has T, return reference
                return GetComponentRef<T>(old_loc);
            }

            // Build new signature
            ComponentSignature new_sig = old_sig;
            new_sig.set(tid, true);

            // Move to new archetype
            Archetype* new_arch = GetOrCreateArchetype(new_sig);
            auto [new_chunk, new_idx] = new_arch->AddEntity(e);

            // Copy old data
            old_arch->CopyEntity(old_loc.chunk_ptr, old_loc.index_in_chunk, new_chunk, new_idx);

            // Remove from old
            old_arch->RemoveEntity(old_loc.chunk_ptr, old_loc.index_in_chunk);

            // Construct T in place
            T tmp{ std::forward<Args>(args)... };
            SetComponentInChunk<T>(new_chunk, new_idx, tmp);

            // Update location
            EntityLocation new_loc;
            new_loc.archetype_ptr = new_arch;
            new_loc.chunk_ptr = new_chunk;
            new_loc.index_in_chunk = new_idx;
            m_entity_locations[id] = new_loc;

            return GetComponentRef<T>(new_loc);
        }

        template <typename T>
        void RemoveComponent(Entity e)
        {
            u32 id = e.GetId();
            if (id >= m_entity_locations.size())
            {
                return;
            }

            EntityLocation old_loc = m_entity_locations[id];
            if (!old_loc.archetype_ptr)
            {
                return;
            }

            Archetype* old_arch = old_loc.archetype_ptr;
            ComponentSignature old_sig = old_arch->GetSignature();

            u32 tid = GetComponentTypeID<T>();
            if (!old_sig.test(tid))
            {
                // doesn't have T
                return;
            }

            ComponentSignature new_sig = old_sig;
            new_sig.set(tid, false);

            Archetype* new_arch = GetOrCreateArchetype(new_sig);
            auto [new_chunk, new_idx] = new_arch->AddEntity(e);

            old_arch->CopyEntity(old_loc.chunk_ptr, old_loc.index_in_chunk, new_chunk, new_idx);
            old_arch->RemoveEntity(old_loc.chunk_ptr, old_loc.index_in_chunk);

            m_entity_locations[id] = { new_arch, new_chunk, new_idx };
        }

        template <typename T>
        T& GetComponent(Entity e)
        {
            u32 id = e.GetId();
            EntityLocation& loc = m_entity_locations[id];
            return GetComponentRef<T>(loc);
        }

        template <typename T>
        bool HasComponent(Entity e) const
        {
            u32 id = e.GetId();
            if (id >= m_entity_locations.size())
            {
                return false;
            }
            const EntityLocation& loc = m_entity_locations[id];
            if (!loc.archetype_ptr)
            {
                return false;
            }
            return loc.archetype_ptr->GetSignature().test(GetComponentTypeID<T>());
        }

        // ----------------------------------------------------------
// Query
// ----------------------------------------------------------
        // --------------------------------------------------------------------------
// Query: collects chunks, then iterates them efficiently
// --------------------------------------------------------------------------
        template <typename... Filters>
        class Query
        {
        public:
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
            private:
                using Tail = typename ExtractIncluded<Rest...>::type;

            public:
                using type = std::conditional_t<
                    IsSpecialization<First, Without>::value,
                    Tail,
                    decltype(std::tuple_cat(std::declval<std::tuple<First>>(), std::declval<Tail>()))
                >;
            };

            using IncludedTypes = typename ExtractIncluded<Filters...>::type;

            struct ChunkView
            {
                Chunk* m_chunk = nullptr;
                const Entity* m_entity_array = nullptr;
                usize m_count = 0;

                // For each included type: pointer to array base + size
                std::array<std::byte*, std::tuple_size<IncludedTypes>::value> m_arrays;
                std::array<usize, std::tuple_size<IncludedTypes>::value> m_sizes;
            };

            Query(const Coordinator& c)
                : m_coord(c)
            {
                ParseFilters<Filters...>();

                // gather archetypes that match
                for (auto& kv : m_coord.m_archetypes)
                {
                    const auto& arch_sig = kv.first;
                    if ((arch_sig & m_include_sig) == m_include_sig)
                    {
                        if ((arch_sig & m_exclude_sig).any())
                            continue;

                        // Gather chunk data
                        Archetype* arch = kv.second.get();
                        GatherChunkViews(arch);
                    }
                }
            }

            // ForEach: pass (entityId, T1&, T2&, ...) to func
            template <typename Func>
            void ForEach(Func func)
            {
                for (auto& cv : m_chunk_views)
                {
                    const Entity* entity_array = cv.m_entity_array;
                    usize count = cv.m_count;

                    for (usize i = 0; i < count; ++i)
                    {
                        Entity e = entity_array[i];
                        // Expand references to each included component
                        CallFunc(e, func, cv, i,
                            std::make_index_sequence<std::tuple_size<IncludedTypes>::value>{});
                    }
                }
            }

        private:
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

            template <typename F>
            void SetFilter()
            {
                if constexpr (IsSpecialization<F, Without>::value)
                {
                    using TT = typename F::type;
                    m_exclude_sig.set(GetComponentTypeID<TT>(), true);
                }
                else
                {
                    m_include_sig.set(GetComponentTypeID<F>(), true);
                }
            }

            void GatherChunkViews(Archetype* arch)
            {
                // We can iterate non-const to get non-const chunk pointers
                for (auto& chunk : arch->GetChunks())
                {
                    if (chunk.Size() == 0)
                        continue;

                    ChunkView cv;
                    cv.m_chunk = &chunk;
                    cv.m_entity_array = chunk.GetEntities().data();
                    cv.m_count = chunk.Size();

                    FillChunkViewArrays(cv,
                        std::make_index_sequence<std::tuple_size<IncludedTypes>::value>{});

                    m_chunk_views.emplace_back(cv);
                }
            }

            template <size_t... I>
            void FillChunkViewArrays(ChunkView& cv, std::index_sequence<I...>)
            {
                (FillOneArray<I, std::tuple_element_t<I, IncludedTypes>>(cv), ...);
            }

            template <size_t IDX, typename T>
            void FillOneArray(ChunkView& cv)
            {
                u32 tid = GetComponentTypeID<T>();
                // Use non-const GetComponentData => returns void*
                void* base_ptr = cv.m_chunk->GetComponentData(tid, 0);
                cv.m_arrays[IDX] = static_cast<std::byte*>(base_ptr);
                cv.m_sizes[IDX] = g_type_sizes[tid];
            }

            // CallFunc: create references for each T from ChunkView => pass to Func
            template <typename Func, size_t... I>
            void CallFunc(const Entity& e,
                Func& func,
                const ChunkView& cv,
                usize idx,
                std::index_sequence<I...>)
            {
                func(
                    e.GetId(),
                    *reinterpret_cast<std::tuple_element_t<I, IncludedTypes>*>(
                        cv.m_arrays[I] + idx * cv.m_sizes[I])...
                );
            }

        private:
            const Coordinator& m_coord;
            ComponentSignature m_include_sig;
            ComponentSignature m_exclude_sig;
            std::vector<ChunkView> m_chunk_views;
        };

        // Query builder
        template <typename... Fs>
        Query<Fs...> CreateQuery() const
        {
            return Query<Fs...>(*this);
        }

    private:
        std::vector<EntityLocation> m_entity_locations;

        // Freed IDs
        std::stack<Entity> m_recycled_ids;
        std::vector<bool> m_active_entities;
        std::vector<u32> m_entity_generations;

        // Archetypes by signature
        std::unordered_map<ComponentSignature, std::unique_ptr<Archetype>> m_archetypes;

    private:
        // create new or reuse old ID
        Entity CreateEmptyEntity()
        {
            if (!m_recycled_ids.empty())
            {
                Entity r = m_recycled_ids.top();
                m_recycled_ids.pop();
                m_entity_generations[r.GetId()]++;
                m_active_entities[r.GetId()] = true;
                return Entity(r.GetId(), m_entity_generations[r.GetId()]);
            }
            else
            {
                u32 new_id = (u32)m_active_entities.size();
                m_active_entities.push_back(true);
                m_entity_generations.push_back(0);
                return Entity(new_id, 0);
            }
        }

        Archetype* GetOrCreateArchetype(const ComponentSignature& sig)
        {
            auto it = m_archetypes.find(sig);
            if (it != m_archetypes.end())
            {
                return it->second.get();
            }
            auto new_arch = std::make_unique<Archetype>(sig);
            Archetype* raw = new_arch.get();
            m_archetypes.insert({ sig, std::move(new_arch) });
            return raw;
        }

        template <typename T>
        void SetComponentInChunk(Chunk* c, usize idx, T&& val)
        {
            u32 tid = GetComponentTypeID<T>();
            void* data = c->GetComponentData(tid, idx);
            std::memcpy(data, &val, sizeof(T));
        }

        template <typename First, typename... Rest>
        void FillComponents(Chunk* chunk_ptr, usize idx, First&& fst, Rest&&... rest)
        {
            SetComponentInChunk<std::remove_reference_t<First>>(chunk_ptr, idx, std::forward<First>(fst));
            if constexpr (sizeof...(rest) > 0)
            {
                FillComponents<Rest...>(chunk_ptr, idx, std::forward<Rest>(rest)...);
            }
        }

        template <typename T>
        T& GetComponentRef(const EntityLocation& loc)
        {
            u32 tid = GetComponentTypeID<T>();
            void* ptr = loc.chunk_ptr->GetComponentData(tid, loc.index_in_chunk);
            return *reinterpret_cast<T*>(ptr);
        }

        void SetEntityLocation(u32 id, EntityLocation loc)
        {
            if (id >= m_entity_locations.size())
            {
                m_entity_locations.resize(id + 1);
            }
            m_entity_locations[id] = loc;
        }
    };
}

#endif // SPARK_ECS_HPP
