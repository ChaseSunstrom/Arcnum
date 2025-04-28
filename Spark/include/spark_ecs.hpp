#ifndef SPARK_ECS_HPP
#define SPARK_ECS_HPP

#include "spark_pch.hpp"
#include "spark_defines.hpp"
#include <cstring>
#include <type_traits>

namespace spark
{
    constexpr usize MAX_COMPONENTS = 64;
    using ComponentSignature = u64;

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

    template <typename T>
    struct Without
    {
        using type = T;
    };

    namespace detail
    {
        template <typename T, template <typename...> class Template>
        struct IsSpecialization : std::false_type {};

        template <template <typename...> class Template, typename... Args>
        struct IsSpecialization<Template<Args...>, Template> : std::true_type {};

        inline u32 GetUniqueTypeID()
        {
            static std::atomic<u32> s_last_id{ 0 };
            return s_last_id++;
        }

        // Move ExtractIncluded to namespace scope
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
    }

    template <typename T>
    u32 GetComponentTypeID() noexcept
    {
        static const u32 s_type_id = detail::GetUniqueTypeID();
        // Ensure we don't exceed MAX_COMPONENTS
        assert(s_type_id < MAX_COMPONENTS);
        return s_type_id;
    }

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
            if constexpr (std::is_trivially_copyable_v<T>)
            {
                g_copy_fns[tid] = nullptr;
            }
            else
            {
                g_copy_fns[tid] = &CopyComponentFn<T>;
            }
        }
    }

    // Chunk: SoA data portion for a given signature
    class Chunk
    {
    public:
        Chunk(const ComponentSignature& signature, usize chunk_size_bytes = CHUNK_SIZE_BYTES)
            : m_signature(signature)
            , m_capacity_bytes(chunk_size_bytes)
        {
            for (int i = 0; i < MAX_COMPONENTS; ++i)
            {
                m_type_map[i] = -1;
            }
        }

        void InitLayout()
        {
            m_type_ids.clear();
            m_type_sizes.clear();
            m_component_offsets.clear();

            // Calculate total_size_per_entity, gather TIDs
            m_total_size_per_entity = 0; // Initialize the member variable
            for (u32 tid = 0; tid < MAX_COMPONENTS; ++tid)
            {
                if ((m_signature >> tid) & 1ULL)
                {
                    usize sz = g_type_sizes[tid];
                    m_type_ids.push_back(tid);
                    m_type_sizes.push_back(sz);
                    m_component_offsets.push_back(m_total_size_per_entity); // Update offsets
                    m_total_size_per_entity += sz;
                }
            }

            // For large component sets, ensure at least N entities per chunk
            if (m_total_size_per_entity > 0)
            {
                constexpr usize min_entities = 64;
                usize needed_bytes = m_total_size_per_entity * min_entities;
                if (needed_bytes > m_capacity_bytes)
                    m_capacity_bytes = needed_bytes;
            }
            if (m_total_size_per_entity == 0)
            {
                m_capacity_entities = m_capacity_bytes;
            }
            else
            {
                m_capacity_entities = m_capacity_bytes / m_total_size_per_entity;
            }

            m_entities.resize(m_capacity_entities);
            m_entity_count = 0;

            // Allocate the data buffer
            m_data.resize(m_capacity_entities * m_total_size_per_entity);

            // Initialize m_type_map
            for (usize i = 0; i < m_type_ids.size(); ++i)
            {
                u32 tid = m_type_ids[i];
                m_type_map[tid] = static_cast<int>(i);
            }

            // Determine if we can memcpy whole entities (all components trivially copyable)
            m_all_trivial = true;
            for (u32 tid : m_type_ids)
            {
                if (g_copy_fns[tid])
                {
                    m_all_trivial = false;
                    break;
                }
            }
        }


        const std::vector<std::byte>& GetData() const
        {
            return m_data;
        }


        bool HasSpace() const noexcept
        {
            return (m_entity_count < m_capacity_entities);
        }

        usize Size() const noexcept
        {
            return m_entity_count;
        }

        Entity GetEntity(usize i) const noexcept
        {
            return m_entities[i];
        }

        usize AddEntity(Entity e) noexcept
        {
            const usize idx = m_entity_count++;
            m_entities[idx] = e;
            return idx;
        }

        void RemoveEntity(usize idx) noexcept
        {
            const usize last_idx = m_entity_count - 1;
            if (idx != last_idx)
            {
                m_entities[idx] = m_entities[last_idx];
                if (m_all_trivial)
                {
                    // fast path: memcpy whole entity
                    std::memcpy(
                        m_data.data() + idx * m_total_size_per_entity,
                        m_data.data() + last_idx * m_total_size_per_entity,
                        m_total_size_per_entity);
                }
                else
                {
                    // slow path: per-component copy or custom fn
                    for (usize i = 0; i < m_type_ids.size(); ++i)
                    {
                        u32 tid = m_type_ids[i];
                        CopyFn fn = g_copy_fns[tid];
                        usize sz = m_type_sizes[i];
                        const usize src_offset = m_component_offsets[i] + last_idx * sz;
                        const usize dst_offset = m_component_offsets[i] + idx * sz;
                        const void* s_ptr = m_data.data() + src_offset;
                        void* d_ptr = m_data.data() + dst_offset;
                        if (fn)
                            fn(s_ptr, d_ptr);
                        else
                            std::memcpy(d_ptr, s_ptr, sz);
                    }
                }
            }
            --m_entity_count;
        }

        usize GetTotalSizePerEntity() const {
            return m_total_size_per_entity;
        }


        usize GetComponentOffset(u32 tid) const
        {
            const i32 arr_index = m_type_map[tid];
            if (arr_index < 0)
            {
                throw std::runtime_error("Component type ID not found in Chunk.");
            }
            return m_component_offsets[arr_index];
        }


        // The SoA "array" for component i is effectively:
        //   base pointer = &m_data[ m_component_offsets[i] + (index * m_type_sizes[i]) ]
        // A swap or copy is just memmove or byte copying.

        void SwapData(usize arr_index, usize idx_a, usize idx_b)
        {
            if (idx_a == idx_b)
            {
                return;
            }
            const usize sz = m_type_sizes[arr_index];
            const usize offset = m_component_offsets[arr_index];
            std::byte* base = m_data.data();

            std::byte* A = base + offset + idx_a * sz;
            std::byte* B = base + offset + idx_b * sz;

            for (usize i = 0; i < sz; ++i)
            {
                std::byte tmp = A[i];
                A[i] = B[i];
                B[i] = tmp;
            }
        }

        void CopyTo(usize idx_src, Chunk* dst_chunk, usize idx_dst) const noexcept
        {
            if (m_all_trivial)
            {
                // fast path: memcpy entire entity slice
                std::memcpy(
                    dst_chunk->m_data.data() + idx_dst * m_total_size_per_entity,
                    m_data.data() + idx_src * m_total_size_per_entity,
                    m_total_size_per_entity);
            }
            else
            {
                // slow path: per-component copy or custom fn
                for (usize i = 0; i < m_type_ids.size(); ++i)
                {
                    u32 tid = m_type_ids[i];
                    CopyFn fn = g_copy_fns[tid];
                    usize sz = m_type_sizes[i];
                    const usize src_offset = m_component_offsets[i] + idx_src * sz;
                    const usize dst_offset = dst_chunk->m_component_offsets[i] + idx_dst * sz;
                    const void* s_ptr = &m_data[src_offset];
                    void* d_ptr = &dst_chunk->m_data[dst_offset];
                    if (fn)
                        fn(s_ptr, d_ptr);
                    else
                        std::memcpy(d_ptr, s_ptr, sz);
                }
            }
        }

        void* GetComponentData(u32 tid, usize idx) noexcept
        {
            int arr_index = m_type_map[tid];
            return (arr_index < 0) ? nullptr : m_data.data() + idx * m_total_size_per_entity + m_component_offsets[arr_index];
        }

        const void* GetComponentData(u32 tid, usize idx) const noexcept
        {
            int arr_index = m_type_map[tid];
            return (arr_index < 0) ? nullptr : m_data.data() + idx * m_total_size_per_entity + m_component_offsets[arr_index];
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

        // Single data buffer
        std::vector<std::byte> m_data;
        usize m_total_size_per_entity = 0;

        // For each type in this chunk, we store a start offset for that type
        //   within an entity's "slice" in m_data
        std::vector<usize> m_component_offsets;

        // So we know how big each type is
        std::vector<u32> m_type_ids;
        std::vector<usize> m_type_sizes;

        // Entities
        std::vector<Entity> m_entities;

        // Flag indicating if entire entity memory can be memcpy'd
        bool m_all_trivial = false;

        // tid -> index in m_type_ids (or -1 if not present)
        i32  m_type_map[MAX_COMPONENTS];
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
            m_entity_locations.reserve(1024);
            m_archetypes.reserve(32);
        }

        Coordinator(const Coordinator&) = delete;
        Coordinator& operator=(const Coordinator&) = delete;

        // Create entity with optional components
        template <typename... Components>
        Entity CreateEntity(Components&&... comps)
        {
            // one-time type init and signature creation per template instantiation
            static const bool inited = []() {
                (InitTypeIfNeeded<std::remove_reference_t<Components>>(), ...);
                return true;
            }(); (void)inited;
            static const ComponentSignature sig =
                ((ComponentSignature(1ULL) << GetComponentTypeID<std::remove_reference_t<Components>>()) | ...);
            Entity e = CreateEmptyEntity();
            Archetype* arch = GetOrCreateArchetype(sig);
            auto [chunk_ptr, idx] = arch->AddEntity(e);
            // fill components if any
            if constexpr (sizeof...(comps) > 0)
                FillComponents<Components...>(chunk_ptr, idx, std::forward<Components>(comps)...);
            SetEntityLocation(e.GetId(), { arch, chunk_ptr, idx });
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
            ComponentSignature new_sig = old_sig | (ComponentSignature(1ULL) << tid);

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

        std::vector<std::pair<ComponentSignature, std::unique_ptr<Archetype>>>& GetArchetypes() {
            return m_archetypes;
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

            ComponentSignature new_sig = old_sig & ~(ComponentSignature(1ULL) << tid);

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

        template <typename... Filters>
        class Query
        {
        public:
            using ComponentTypes = std::tuple<Filters...>;

            using IncludedTypes = typename detail::ExtractIncluded<Filters...>::type;

            // The result type of this query (single component type or tuple of types)
            using result_type = std::conditional_t<
                (std::tuple_size<IncludedTypes>::value == 1),
                std::tuple_element_t<0, IncludedTypes>,
                IncludedTypes
            >;

            struct ChunkView
            {
                Chunk* m_chunk = nullptr;
                const Entity* m_entity_array = nullptr;
                usize m_count = 0;

                // Pointer to the start of the data buffer
                std::byte* m_data = nullptr;

                // Total size of all components per entity
                usize m_total_size_per_entity = 0;

                // Offsets for each component within an entity's data slice
                std::array<usize, std::tuple_size<IncludedTypes>::value> m_component_offsets;
            };


            Query(Coordinator& c)
                : m_coord(c)
                , m_include_sig(0)
                , m_exclude_sig(0)
            {
                // avoid reallocations when gathering chunk views
                m_chunk_views.reserve(m_coord.GetArchetypes().size());
                ParseFilters<Filters...>();
                // Gather matching archetypes
                for (auto& kv : m_coord.GetArchetypes())
                {
                    const auto& arch_sig = kv.first;
                    if ((arch_sig & m_include_sig) == m_include_sig)
                    {
                        if ((arch_sig & m_exclude_sig) != 0ULL)
                            continue;

                        // Gather chunk data
                        Archetype* arch = kv.second.get();
                        GatherChunkViews(arch);
                    }
                }
            }

            // ForEach: pass (entityId, T1&, T2&, ...) to func
            template <typename Func>
            void ForEach(Func func) const noexcept
            {
                ForEachImpl<Func>(func, std::make_index_sequence<std::tuple_size<IncludedTypes>::value>{});
            }

            // Optimized implementation: precompute per-component base pointers and advance by stride
            template <typename Func, size_t... I>
            void ForEachImpl(Func func, std::index_sequence<I...>) const noexcept
            {
                for (auto& cv : m_chunk_views)
                {
                    const Entity* entity_array = cv.m_entity_array;
                    usize count = cv.m_count;
                    usize stride = cv.m_total_size_per_entity;
                    std::array<std::byte*, sizeof...(I)> bases = { (cv.m_data + cv.m_component_offsets[I])... };
                    for (usize j = 0; j < count; ++j)
                    {
                        func(entity_array[j], *reinterpret_cast<std::tuple_element_t<I, IncludedTypes>*>(bases[I])...);
                        ((bases[I] += stride), ...);
                    }
                }
            }

            usize Size() const noexcept
            {
                usize total = 0;
                for (auto& cv : m_chunk_views)
                    total += cv.m_count;
                return total;
            }

            inline std::vector<result_type> GetVector() const noexcept
            {
                std::vector<result_type> result_vector;
                result_vector.reserve(Size());
                for (const auto& cv : m_chunk_views)
                    for (usize idx = 0; idx < cv.m_count; ++idx)
                        result_vector.emplace_back(GetResult(cv, idx));
                return result_vector;
            }

            // Iterator support for ranged-for
            class Iterator
            {
            public:
                inline Iterator(const Query* q, bool end = false) noexcept
                    : m_query(q), m_view_idx(0), m_idx(0)
                {
                    if (end || q->m_chunk_views.empty())
                    {
                        m_view_idx = q->m_chunk_views.size();
                        m_idx = 0;
                    }
                }

                using result_type = typename Query::result_type;

                inline result_type operator*() const noexcept
                {
                    const auto& cv = m_query->m_chunk_views[m_view_idx];
                    return m_query->GetResult(cv, m_idx);
                }

                inline Iterator& operator++() noexcept
                {
                    const auto& cv = m_query->m_chunk_views[m_view_idx];
                    if (++m_idx >= cv.m_count)
                    {
                        ++m_view_idx;
                        m_idx = 0;
                    }
                    if (m_view_idx >= m_query->m_chunk_views.size())
                    {
                        m_view_idx = m_query->m_chunk_views.size();
                        m_idx = 0;
                    }
                    return *this;
                }

                inline bool operator!=(const Iterator& other) const noexcept
                {
                    return m_view_idx != other.m_view_idx || m_idx != other.m_idx;
                }

            private:
                const Query* m_query;
                usize m_view_idx;
                usize m_idx;
            };

            inline Iterator begin() const noexcept { return Iterator(this, false); }
            inline Iterator end() const noexcept { return Iterator(this, true); }

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
                if constexpr (detail::IsSpecialization<F, Without>::value)
                {
                    using TT = typename F::type;
                    m_exclude_sig |= ComponentSignature(1ULL) << GetComponentTypeID<TT>();
                }
                else
                {
                    m_include_sig |= ComponentSignature(1ULL) << GetComponentTypeID<F>();
                }
            }

            // Helper: Given a ChunkView and an entity index, build a tuple of component values
            // by reading the data at the proper offsets.
            template <std::size_t... idxs>
            auto GetTupleFromChunkView(const ChunkView& chunk_view, usize idx, std::index_sequence<idxs...>) const
            {
                return std::make_tuple(
                    *reinterpret_cast<std::tuple_element_t<idxs, IncludedTypes>*>(
                        chunk_view.m_data + idx * chunk_view.m_total_size_per_entity + chunk_view.m_component_offsets[idxs]
                        )...
                );
            }

            // Helper: For a given ChunkView and entity index, return the result.
            // If exactly one component is queried, return that component's value;
            // otherwise, return a tuple of the values.
            auto GetResult(const ChunkView& chunk_view, usize idx) const
            {
                if constexpr (std::tuple_size<IncludedTypes>::value == 1)
                {
                    auto tuple_val = GetTupleFromChunkView(chunk_view, idx, std::make_index_sequence<1>{});
                    return std::get<0>(tuple_val);
                }
                else
                {
                    return GetTupleFromChunkView(chunk_view, idx, std::make_index_sequence<std::tuple_size<IncludedTypes>::value>{});
                }
            }

            void GatherChunkViews(Archetype* arch)
            {
                for (auto& chunk : arch->GetChunks())
                {
                    if (chunk.Size() == 0)
                        continue;

                    ChunkView cv;
                    cv.m_chunk = &chunk;
                    cv.m_entity_array = chunk.GetEntities().data();
                    cv.m_count = chunk.Size();

                    // Retrieve the data buffer from the Chunk
                    const std::vector<std::byte>& data = chunk.GetData();
                    if (data.empty())
                    {
                        // Handle empty data buffer
                        cv.m_data = nullptr;
                    }
                    else
                    {
                        // Assign the data pointer (const_cast is used to remove constness if necessary)
                        cv.m_data = const_cast<std::byte*>(data.data());
                    }

                    cv.m_total_size_per_entity = chunk.GetTotalSizePerEntity();

                    // Populate component offsets
                    FillChunkViewArrays(cv,
                        std::make_index_sequence<std::tuple_size<IncludedTypes>::value>{});

                    m_chunk_views.emplace_back(cv);
                }
            }

            template <size_t... I>
            void FillChunkViewArrays(ChunkView& cv, std::index_sequence<I...>)
            {
                // Retrieve total size per entity from the chunk
                cv.m_total_size_per_entity = cv.m_chunk->GetTotalSizePerEntity();

                // Populate component offsets
                (FillOneArray<I, std::tuple_element_t<I, IncludedTypes>>(cv), ...);
            }

            template <size_t IDX, typename T>
            void FillOneArray(ChunkView& cv)
            {
                u32 tid = GetComponentTypeID<T>();
                // Retrieve the offset of component T within the entity's data slice
                usize offset = cv.m_chunk->GetComponentOffset(tid);
                cv.m_component_offsets[IDX] = offset;
            }

        private:
            Coordinator& m_coord;
            ComponentSignature m_include_sig;
            ComponentSignature m_exclude_sig;
            std::vector<ChunkView> m_chunk_views;
        };

        // Query builder
        template <typename... Fs>
        Query<Fs...> CreateQuery() 
        {
            return Query<Fs...>(*this);
        }

    private:
        std::vector<EntityLocation> m_entity_locations;

        // Freed IDs
        std::stack<Entity> m_recycled_ids;
        std::vector<bool> m_active_entities;
        std::vector<u32> m_entity_generations;

        // Archetypes by signature; use vector for faster small-count linear search
        std::vector<std::pair<ComponentSignature, std::unique_ptr<Archetype>>> m_archetypes;

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
                m_entity_locations.emplace_back();
                return Entity(r.GetId(), m_entity_generations[r.GetId()]);
            }
            else
            {
                u32 new_id = (u32)m_active_entities.size();
                m_active_entities.push_back(true);
                m_entity_generations.push_back(0);
                m_entity_locations.emplace_back();
                return Entity(new_id, 0);
            }
        }

        Archetype* GetOrCreateArchetype(const ComponentSignature& sig)
        {
            // Linear search since typical archetype count is small
            for (auto& entry : m_archetypes)
            {
                if (entry.first == sig)
                    return entry.second.get();
            }
            auto new_arch = std::make_unique<Archetype>(sig);
            Archetype* raw = new_arch.get();
            m_archetypes.emplace_back(sig, std::move(new_arch));
            return raw;
        }

        template <typename T>
        void SetComponentInChunk(Chunk* c, usize idx, const T& val)
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

    template <typename... Filters>
    using Query = Coordinator::Query<Filters...>;
}

#endif // SPARK_ECS_HPP