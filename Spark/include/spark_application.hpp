#ifndef SPARK_APPLICATION_HPP
#define SPARK_APPLICATION_HPP

#include "spark_pch.hpp"
#include "spark_ecs.hpp"
#include "spark_window.hpp"
#include "spark_event.hpp"
#include "spark_event_queue.hpp"
#include "spark_threading.hpp"
#include "spark_stopwatch.hpp"
#include "spark_graphics_api.hpp"
#include "spark_defer.hpp"
#include "spark_layer_stack.hpp"
#include "spark_renderer_layer.hpp"
#include "spark_command_queue.hpp"
#include "spark_delta_time.hpp"
#include "spark_event_layer.hpp"
#include "special/spark_modding.hpp"

namespace spark
{
    template <typename T>
    struct IsSparkQuery : std::false_type {};

    template <typename... Ts>
    struct IsSparkQuery<Query<Ts...>> : std::true_type {};
    
    // If you want an aggregator that references your Coordinator's Query, 
    // define something like:
    //   using RealQuery<Ts...> = Coordinator::Query<Ts...>;
    // Then build RealQuery inside your aggregator from lockedCoord.
    template <typename T>
    struct IsEvent : std::false_type {};

    // If you have a multi-event type
    template <typename... Ts>
    struct IsEvent<Event<Ts...>> : std::true_type {};

    // ParamTrait
    template <typename T>
    struct ParamTrait
    {
        using Decayed = std::remove_cv_t<std::remove_reference_t<T>>;
        static constexpr bool is_query = IsSparkQuery<Decayed>::value;
        static constexpr bool is_event = IsEvent<Decayed>::value;
        static constexpr bool is_resource = (!is_query && !is_event);
    };

    // If user wants to build SparkQuery<Ts...> from a coordinator
    template <typename T>
    struct SparkQueryComponents; // specialized for SparkQuery

    template <typename... Ts>
    struct SparkQueryComponents<Query<Ts...>>
    {
        static Query<Ts...> Get(Coordinator& coord)
        {
            return coord.CreateQuery<Ts...>();
        }
    };

    // SystemPhase, SystemExecutionMode, SystemSettings
    enum class SystemPhase
    {
        ON_START,
        BEFORE_UPDATE,
        UPDATE,
        AFTER_UPDATE,
        ON_SHUTDOWN
    };

    enum class SystemExecutionMode
    {
        SINGLE_THREADED,
        MULTITHREADED_SYNC,
        MULTITHREADED_ASYNC
    };

    using SystemPriority = threading::TaskPriority;

    struct SystemSettings
    {
        SystemPhase phase = SystemPhase::UPDATE;
        SystemExecutionMode execution_mode = SystemExecutionMode::SINGLE_THREADED;
        SystemPriority priority = SystemPriority::NORMAL;
    };

    // FunctionTraits for analyzing system function signatures
    template <typename R, typename... Args>
    struct FunctionTraitsImpl
    {
        using return_type = R;
        using args_tuple = std::tuple<Args...>;
        static constexpr size_t arity = sizeof...(Args);

        template <size_t Index>
        using arg = std::tuple_element_t<Index, args_tuple>;
    };

    template <typename T, typename = void>
    struct FunctionTraits;

    // For free functions or static member functions
    template <typename R, typename... Args>
    struct FunctionTraits<R(Args...), void> : FunctionTraitsImpl<R, Args...> {};

    // e.g. function pointer
    template <typename R, typename... Args>
    struct FunctionTraits<R(*)(Args...), void> : FunctionTraitsImpl<R, Args...> {};

    // e.g. class member
    template <typename C, typename R, typename... Args>
    struct FunctionTraits<R(C::*)(Args...), void> : FunctionTraitsImpl<R, Args...> {};

    template <typename C, typename R, typename... Args>
    struct FunctionTraits<R(C::*)(Args...) const, void> : FunctionTraitsImpl<R, Args...> {};

    // e.g. functor with operator()
    template <typename T>
    struct FunctionTraits<T, std::void_t<decltype(&T::operator())>>
        : FunctionTraits<decltype(&T::operator())>
    { };

    namespace detail
    {
        template <typename T, bool IsResource>
        struct ArgHolder;

        // ArgHolder for "not a resource" (the second template param is `false`)
        template <typename T>
        struct ArgHolder<T, false>
        {
            T m_value;
            explicit ArgHolder(T&& val)
                : m_value(std::move(val))
            {
            }
            T Get()
            {
                return m_value;
            }
        };

        // ArgHolder for "resource by value" (second param is `true`)
        template <typename T>
        struct ArgHolder<T, true>
        {
            using resource_type = std::remove_reference_t<T>;

            // 2) Fully qualify the locked references:
            using lock_type = std::conditional_t<
                std::is_const_v<resource_type>,
                spark::threading::LockedCRef<std::remove_const_t<resource_type>>,
                spark::threading::LockedRef<resource_type>
            >;

            std::shared_ptr<lock_type> m_lock;

            explicit ArgHolder(std::shared_ptr<lock_type> lk)
                : m_lock(std::move(lk))
            {
            }

            T Get()
            {
                // For a value: we copy out the underlying resource
                return m_lock->Get();
            }
        };

        // ArgHolder for "resource by reference" => T is e.g. SomeType&
        template <typename U>
        struct ArgHolder<U&, true>
        {
            using resource_type = std::remove_reference_t<U>;

            using lock_type = std::conditional_t<
                std::is_const_v<resource_type>,
                spark::threading::LockedCRef<std::remove_const_t<resource_type>>,
                spark::threading::LockedRef<resource_type>
            >;

            std::shared_ptr<lock_type> m_lock;

            explicit ArgHolder(std::shared_ptr<lock_type> lk)
                : m_lock(std::move(lk))
            {
            }

            U& Get()
            {
                return m_lock->Get();
            }
        };
    } // end namespace spark::detail

    // ------------------------------------------------------------------------
    // Forward Declarations
    // ------------------------------------------------------------------------
    class Application;

    // For convenience
    template <typename T>
    using Ref = T&;

    template <typename T>
    using CRef = const T&;

    namespace detail
    {
        struct ISystem
        {
            virtual ~ISystem() = default;
            virtual void Execute(Application& app) = 0;
            virtual void Execute(Application& app, std::shared_ptr<IEvent> forced_event) = 0;
            virtual SystemPriority GetPriority() const = 0;
            virtual SystemExecutionMode GetExecutionMode() const = 0;
        };

        template <typename Func>
        struct System;
    }

    // ------------------------------------------------------------------------
    // The Application class
    // ------------------------------------------------------------------------
    class Application
    {
    public:
        Application(
            GraphicsApi gapi,
            const std::string& title,
            int32_t win_width,
            int32_t win_height,
            bool vsync = false
        )
            : m_gapi(gapi)
            , m_mod_manager(this)
        {
            // Basic layering
            m_layer_stack.PushLayer<WindowLayer>(gapi, title, win_width, win_height, vsync);
            m_layer_stack.PushLayer<RendererLayer>(gapi, m_command_queue);
            m_layer_stack.PushLayer<EventLayer>(*this, m_event_queue);

            // Add these resources
            AddResource(*this);
            AddResource(m_coordinator);
            AddResource(m_thread_pool);
        }

        ~Application()
        {
            Close();
        }

        Application& AddThreads(std::size_t num_threads)
        {
            m_thread_pool.AddThreads(num_threads);
            return *this;
        }

        Application& Start()
        {
            DispatchSystemsForPhase(SystemPhase::ON_START);
            m_layer_stack.Start();
            return *this;
        }

        Application& Run()
        {
            while (IsRunning())
            {
                DispatchSystemsForPhase(SystemPhase::BEFORE_UPDATE);
                m_layer_stack.Update(m_dt);
                DispatchSystemsForPhase(SystemPhase::UPDATE);
                DispatchSystemsForPhase(SystemPhase::AFTER_UPDATE);
            }
            return *this;
        }

        Application& Close()
        {
            DispatchSystemsForPhase(SystemPhase::ON_SHUTDOWN);
            m_layer_stack.Stop();
            m_thread_pool.Shutdown();
            return *this;
        }

        bool IsRunning()
        {
            return GetWindowLayer().Running();
        }

        // Example mod loader
        Application& LoadMod(const std::string& mod_path)
        {
            auto handle = m_mod_manager.LoadLibrary(mod_path);
            if (!handle)
                return *this;
            auto mod_instance = m_mod_manager.LoadModInstance(handle);
            if (!mod_instance)
            {
                m_mod_manager.UnloadLibrary(handle);
            }
            return *this;
        }

        template <typename... Funcs>
        Application& RegisterSystems(Funcs &&... funcs)
        {
            static_assert(sizeof...(Funcs) > 0, "RegisterSystems requires at least one function.");

            using LastType = std::decay_t<std::tuple_element_t<sizeof...(Funcs) - 1, std::tuple<Funcs...>>>;
            constexpr bool last_is_settings = std::is_same_v<LastType, SystemSettings>;

            if constexpr (last_is_settings)
            {
                constexpr size_t n = sizeof...(Funcs) - 1;
                auto args_tuple = std::forward_as_tuple(std::forward<Funcs>(funcs)...);
                const SystemSettings& settings = std::get<n>(args_tuple);
                RegisterSystemsImpl(args_tuple, std::make_index_sequence<n>{}, settings);
            }
            else
            {
                RegisterSystems(std::forward<Funcs>(funcs)..., SystemSettings{});
            }
            return *this;
        }

        template <typename Tuple, size_t... Is>
        Application& RegisterSystemsImpl(Tuple&& tup, std::index_sequence<Is...>, const SystemSettings& settings)
        {
            (RegisterSystem(std::get<Is>(tup), settings), ...);
            return *this;
        }

        template <typename Func>
        Application& RegisterSystem(Func&& system_func, const SystemSettings& settings = {})
        {
            using F = std::decay_t<Func>;
            using Traits = FunctionTraits<F>;
            constexpr size_t arity = Traits::arity;

            bool has_event = false;
            ForSequence(std::make_index_sequence<arity>{}, [&](auto i)
                {
                    using ArgT = typename Traits::template arg<i>;
                    if constexpr (ParamTrait<ArgT>::is_event)
                    {
                        has_event = true;
                    }
                });

            auto system_obj = std::make_unique<detail::System<F>>(std::forward<Func>(system_func), settings);

            if (has_event)
            {
                // If the system has an event param, store it in m_event_systems
                m_event_systems.push_back(std::move(system_obj));
            }
            else
            {
                // Normal system
                m_systems[settings.phase].push_back(std::move(system_obj));
            }
            return *this;
        }

        template <typename... Ts>
        Application& SubmitEvent(const Event<Ts...>& ev)
        {
            m_event_queue.SubmitEvent<Ts...>(ev);
            return *this;
        }

        template <typename T>
        Application& SubmitEvent(const T& ev)
        {
            Event<T> single(ev);
            m_event_queue.SubmitEvent(single);
            return *this;
        }

        // Resource management
        template <typename T>
        bool HasResource() const
        {
            using pointer_type = std::remove_reference_t<T>*;
            return (m_resources.find(std::type_index(typeid(pointer_type))) != m_resources.end());
        }

        template <typename T>
        threading::AutoLockable<T*>& GetResource()
        {
            using pointer_type = std::remove_reference_t<T>*;
            auto it = m_resources.find(std::type_index(typeid(pointer_type)));
            if (it == m_resources.end())
            {
                throw std::runtime_error("Resource not found for type: " + std::string(typeid(T).name()));
            }
            auto ptr_any = std::any_cast<std::shared_ptr<threading::AutoLockable<pointer_type>>>(it->second);
            if (!ptr_any)
            {
                throw std::runtime_error("any_cast failed for " + std::string(typeid(T).name()));
            }
            return *ptr_any;
        }

        template <typename T>
        Application& AddResource(T& obj)
        {
            using pointer_type = std::remove_reference_t<T>*;
            m_resources[std::type_index(typeid(pointer_type))] =
                std::make_shared<threading::AutoLockable<pointer_type>>(&obj);
            return *this;
        }

        Application& SetDeltaTime(DeltaTime<double> dt)
        {
            m_dt = dt;
            return *this;
        }

        Coordinator& GetCoordinator()
        {
            return m_coordinator;
        }

        threading::ThreadPool& GetThreadPool()
        {
            return m_thread_pool;
        }

        void DispatchSystemsForPhase(SystemPhase phase)
        {
            auto it = m_systems.find(phase);
            if (it == m_systems.end())
                return;

            std::vector<std::future<void>> sync_tasks;
            for (auto& sys_ptr : it->second)
            {
                switch (sys_ptr->GetExecutionMode())
                {
                case SystemExecutionMode::SINGLE_THREADED:
                    sys_ptr->Execute(*this);
                    break;

                case SystemExecutionMode::MULTITHREADED_SYNC:
                {
                    auto task = m_thread_pool.Enqueue(
                        sys_ptr->GetPriority(),
                        [this, &sys_ptr]()
                        {
                            sys_ptr->Execute(*this);
                        }
                    );
                    sync_tasks.push_back(std::move(task.result));
                    break;
                }

                case SystemExecutionMode::MULTITHREADED_ASYNC:
                    m_thread_pool.Enqueue(
                        sys_ptr->GetPriority(),
                        [this, &sys_ptr]()
                        {
                            sys_ptr->Execute(*this);
                        }
                    );
                    break;
                }
            }
            for (auto& f : sync_tasks)
            {
                f.get();
            }
        }

        // If you have a renderer
        IRenderer& GetRenderer()
        {
            return GetRendererLayer().GetRenderer();
        }

        Application& ChangeGraphicsApi(GraphicsApi new_gapi)
        {
            if (new_gapi == m_gapi)
                return *this;
            GetWindowLayer().SetGraphicsApi(new_gapi);
            GetRendererLayer().SetGraphicsApi(new_gapi);
            return *this;
        }

        std::vector<std::unique_ptr<detail::ISystem>>& GetEventSystems() { return m_event_systems; }

    private:
        template <size_t... Is, typename F>
        static void ForSequence(std::index_sequence<Is...>, F&& func)
        {
            (func(std::integral_constant<size_t, Is>{}), ...);
        }

        WindowLayer& GetWindowLayer()
        {
            return *m_layer_stack.GetLayer<WindowLayer>();
        }

        RendererLayer& GetRendererLayer()
        {
            return *m_layer_stack.GetLayer<RendererLayer>();
        }

        EventLayer& GetEventLayer()
        {
            return *m_layer_stack.GetLayer<EventLayer>();
        }

    private:
        GraphicsApi m_gapi;
        ModManager m_mod_manager;
        LayerStack m_layer_stack;
        CommandQueue m_command_queue;
        EventQueue m_event_queue;
        threading::ThreadPool m_thread_pool;
        DeltaTime<double> m_dt;
        Coordinator m_coordinator;

        // Systems
        std::unordered_map<SystemPhase, std::vector<std::unique_ptr<detail::ISystem>>> m_systems;
        std::vector<std::unique_ptr<detail::ISystem>> m_event_systems;

        // Resource storage: typeid(...) -> shared_ptr<AutoLockable<...>>
        std::unordered_map<std::type_index, std::any> m_resources;

        // no copy/move
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;
    };
    
    namespace detail
    {
        struct ResourceAggregator
        {
            // For each resource type, we store a shared_ptr<...> in std::any
            // e.g. std::any of std::shared_ptr<LockedRef<Coordinator>>
            std::map<std::type_index, std::any> resource_map;

            bool coordinator_needed = false;
            std::any coordinator_any; // same idea: a shared_ptr<LockedRef<Coordinator>>
        };

        // helper that assigns the underlying shared_ptr into evt if tid matches one of the types
        template <typename ... ts, typename t>
        bool SetVariant(Event<ts...>& evt, t* typed_ptr)
        {
            if (typed_ptr == nullptr)
            {
                return false;
            }
            // Instead of using a no‐op deleter, create a new, owned copy of the event data.
            std::shared_ptr<t> sp = std::make_shared<t>(*typed_ptr);
            evt = Event<ts...>(sp);
            return true;
        }

        template <typename ... ts>
        bool AssignVariantToEvent(Event<ts...>& evt, std::type_index tid, void* raw_ptr)
        {
            bool assigned = false;
            // Use a fold expression over the types ts...
            ((tid == std::type_index(typeid(ts))
                ? (assigned = SetVariant(evt, reinterpret_cast<ts*>(raw_ptr)))
                : false) || ...);
            return assigned;
        }


        template <typename t_expected>
        std::shared_ptr<t_expected> ConvertForcedEvent(std::shared_ptr<IEvent> forced)
        {
            // First try a direct dynamic cast.
            auto casted = std::dynamic_pointer_cast<t_expected>(forced);
            if (casted)
            {
                return casted;
            }

            // Otherwise, assume t_expected is a multi-event.
            // Create a temporary t_expected via its default constructor.
            t_expected temp_evt;
            bool assigned = false;

            // Use forced->VisitActive to extract the active payload.
            forced->VisitActive([&](std::type_index tid, void* raw_ptr)
                {
                    assigned = AssignVariantToEvent(temp_evt, tid, raw_ptr);
                });

            if (!assigned)
            {
                return nullptr;
            }

            return std::make_shared<t_expected>(std::move(temp_evt));
        }


        template <typename Arg>
        void MarkNeededResources(ResourceAggregator& agg)
        {
            if constexpr (ParamTrait<Arg>::is_event)
            {
                // no resource
            }
            else if constexpr (ParamTrait<Arg>::is_query)
            {
                agg.coordinator_needed = true;
            }
            else if constexpr (ParamTrait<Arg>::is_resource)
            {
                using decayed = std::remove_reference_t<Arg>;
                agg.resource_map[std::type_index(typeid(decayed))] = std::any{};
            }
        }

        inline void LockResources(Application& app, ResourceAggregator& agg)
        {
            // If aggregator.coordinator_needed => lock coordinator
            if (agg.coordinator_needed)
            {
                auto& coord_lockable = app.GetResource<Coordinator>();
                auto locked_obj = coord_lockable.Lock();
                // locked_obj is a LockedRef<Coordinator>, 
                // we want a shared_ptr<LockedRef<Coordinator>>
                using lock_type = threading::LockedRef<Coordinator>;
                auto sp = std::make_shared<lock_type>(std::move(locked_obj));
                agg.coordinator_any = std::move(sp);
            }

            // Lock the other resources in sorted order
            std::vector<std::type_index> vec;
            vec.reserve(agg.resource_map.size());
            for (auto& kv : agg.resource_map)
            {
                vec.push_back(kv.first);
            }
            std::sort(vec.begin(), vec.end(), [](auto& a, auto& b) { return a < b; });

            for (auto& ti : vec)
            {
                if (ti == std::type_index(typeid(Coordinator)))
                {
                    if (agg.coordinator_needed)
                    {
                        // reuse aggregator.coordinator_any
                        agg.resource_map[ti] = agg.coordinator_any;
                    }
                    else
                    {
                        // user might have Arg=Coordinator& but no queries
                        auto& co_lockable = app.GetResource<Coordinator>();
                        auto locked_obj = co_lockable.Lock();
                        using lock_type = threading::LockedRef<Coordinator>;
                        auto sp = std::make_shared<lock_type>(std::move(locked_obj));
                        agg.resource_map[ti] = std::move(sp);
                    }
                }
                else if (ti == std::type_index(typeid(Application)))
                {
                    // user might have Arg=Application& or Arg=Application
                    auto& a_lockable = app.GetResource<Application>();
                    auto locked_obj = a_lockable.Lock();
                    using lock_type = threading::LockedRef<Application>;
                    auto sp = std::make_shared<lock_type>(std::move(locked_obj));
                    agg.resource_map[ti] = std::move(sp);
                }
                else if (ti == std::type_index(typeid(threading::ThreadPool)))
                {
                    auto& pool_lockable = app.GetResource<threading::ThreadPool>();
                    auto locked_obj = pool_lockable.Lock();
                    using lock_type = threading::LockedRef<threading::ThreadPool>;
                    auto sp = std::make_shared<lock_type>(std::move(locked_obj));
                    agg.resource_map[ti] = std::move(sp);
                }
                else
                {
                    auto s = ti.name();
                    throw std::runtime_error("Unknown resource type => " + std::string(s));
                }
            }
        }

        template <typename Arg>
        auto BuildParam(Application& app, ResourceAggregator& agg, std::shared_ptr<IEvent> forced);

        // For queries, we store locked coordinator + build SparkQuery
        template <typename T>
        struct QueryArgHolder
        {
            // we also store the locked coordinator
            std::shared_ptr<threading::LockedRef<Coordinator>> coord_locked;
            T query_obj;

            QueryArgHolder(std::shared_ptr<threading::LockedRef<Coordinator>> c, T&& q)
                : coord_locked(std::move(c))
                , query_obj(std::move(q))
            {
            }

            T Get()
            {
                // returns the built query object
                return query_obj;
            }
        };

        template <typename Arg>
        auto BuildParam(Application& app, ResourceAggregator& agg, std::shared_ptr<IEvent> forced)
        {
            using decayed = std::remove_reference_t<Arg>;
            if constexpr (ParamTrait<Arg>::is_event)
            {
                if (!forced)
                {
                    throw std::runtime_error("Event param but no forced event!");
                }
                auto casted = ConvertForcedEvent<decayed>(forced);
                if (!casted)
                {
                    throw std::runtime_error("Could not convert forced event param to expected type!");
                }
                // Now we create an ArgHolder for the event.
                return ArgHolder<Arg, false>(std::move(*casted));
            }

            else if constexpr (ParamTrait<Arg>::is_query)
            {
                // aggregator.coordinator_any is a shared_ptr<LockedRef<Coordinator>>
                if (!agg.coordinator_needed)
                    throw std::runtime_error("Query param but aggregator says no coordinator??");

                using lock_ptr = std::shared_ptr<threading::LockedRef<Coordinator>>;
                auto sp_co = std::any_cast<lock_ptr>(&agg.coordinator_any);
                if (!sp_co || !(*sp_co))
                    throw std::runtime_error("No coordinator locked for query param!");

                // build SparkQuery from coordinator
                auto& locked_co = **sp_co; // lockedCo is a LockedRef<Coordinator>
                auto& real_co = locked_co.Get();
                auto query_obj = SparkQueryComponents<decayed>::Get(real_co);

                return QueryArgHolder<decayed>(*sp_co, std::move(query_obj));
            }
            else if constexpr (ParamTrait<Arg>::is_resource)
            {
                // aggregator.resource_map has a shared_ptr<LockedRef<T>>
                auto ti = std::type_index(typeid(decayed));
                auto it = agg.resource_map.find(ti);
                if (it == agg.resource_map.end())
                    throw std::runtime_error("No aggregator entry for resource => " + std::string(typeid(Arg).name()));

                // e.g. Arg=Coordinator& => store shared_ptr<LockedRef<Coordinator>>
                if constexpr (std::is_reference_v<Arg>)
                {
                    // Arg is e.g. Coordinator& or const Coordinator&
                    // so ArgHolder<Coordinator&,true>
                    // we need a shared_ptr<LockedRef<Coordinator>> or lockedCRef
                    using lock_type = std::conditional_t<
                        std::is_const_v<decayed>,
                        threading::LockedCRef<std::remove_const_t<decayed>>,
                        threading::LockedRef<decayed>
                    >;
                    using sp_type = std::shared_ptr<lock_type>;
                    auto sp = std::any_cast<sp_type>(&it->second);
                    if (!sp || !(*sp))
                        throw std::runtime_error("any_cast to shared_ptr<LockedRef<T>> failed for " + std::string(typeid(Arg).name()));

                    return ArgHolder<Arg, true>(*sp);
                }
                else
                {
                    // Arg is a value type e.g. Coordinator or ThreadPool
                    using lock_type = std::conditional_t<
                        std::is_const_v<decayed>,
                        threading::LockedCRef<std::remove_const_t<decayed>>,
                        threading::LockedRef<decayed>
                    >;
                    using sp_type = std::shared_ptr<lock_type>;
                    auto sp = std::any_cast<sp_type>(&it->second);
                    if (!sp || !(*sp))
                        throw std::runtime_error("any_cast to shared_ptr<threading::LockedRef<T>> failed for " + std::string(typeid(Arg).name()));

                    return ArgHolder<Arg, true>(*sp);
                }
            }
            else
            {
                static_assert(sizeof(Arg) == 0, "Unhandled aggregator param type!");
            }
        }

        // final aggregator-based System
        template <typename Func>
        struct System : public ISystem
        {
            Func m_func;
            SystemExecutionMode m_mode;
            SystemPriority m_priority;

            System(Func&& f, const SystemSettings& st)
                : m_func(std::forward<Func>(f))
                , m_mode(st.execution_mode)
                , m_priority(st.priority)
            { }

            void Execute(Application& app) override
            {
                ExecuteImpl(app, nullptr);
            }

            void Execute(Application& app, std::shared_ptr<IEvent> forced) override
            {
                ExecuteImpl(app, forced);
            }

            SystemPriority GetPriority() const override { return m_priority;  }

            SystemExecutionMode GetExecutionMode() const override
            {
                return m_mode;
            }

        private:
            void ExecuteImpl(Application& app, std::shared_ptr<IEvent> forced)
            {
                CallFunc(app, forced);
            }

            template <size_t... Is, typename F>
            static void ForSequence(std::index_sequence<Is...>, F&& fn)
            {
                (fn(std::integral_constant<size_t, Is>{}), ...);
            }

            void CallFunc(Application& app, std::shared_ptr<IEvent> forced)
            {
                using traits = FunctionTraits<Func>;
                constexpr size_t N = traits::arity;

                ResourceAggregator aggregator;

                ForSequence(std::make_index_sequence<N>{}, [&](auto i) {
                    using ArgT = typename traits::template arg<i>;
                    MarkNeededResources<ArgT>(aggregator);
                    });

                LockResources(app, aggregator);

                auto holders = BuildHolders<typename traits::args_tuple>(
                    app, aggregator, forced, std::make_index_sequence<N>{});

                std::apply(
                    [this](auto&... holder) {
                        m_func(holder.Get()...);
                    },
                    holders
                );
            }

            // We'll build a tuple of (ArgHolder<Arg1>..., ArgHolder<Arg2>..., ...)
            template <typename ArgsTuple, size_t... Is>
            auto BuildHolders(Application& app, ResourceAggregator& aggregator,
                std::shared_ptr<IEvent> forced,
                std::index_sequence<Is...>)
            {
                return std::make_tuple(
                    BuildParam<typename std::tuple_element<Is, ArgsTuple>::type>(app, aggregator, forced)...
                );
            }
        };
    } // end namespace spark::detail
} // namespace spark

#endif // SPARK_APPLICATION_HPP
