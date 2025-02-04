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

#include <any>
#include <typeindex>
#include <memory>
#include <unordered_map>

namespace spark
{
    // Forward
    class Application;

    // -----------------------------------------------------------
    // SystemPhase, SystemExecutionMode, SystemSettings
    // -----------------------------------------------------------
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

    // -----------------------------------------------------------
    // FunctionTraits for analyzing system function signatures
    // -----------------------------------------------------------
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

    // e.g. free function
    template <typename R, typename... Args>
    struct FunctionTraits<R(Args...), void> : FunctionTraitsImpl<R, Args...> {};

    // e.g. function pointer
    template <typename R, typename... Args>
    struct FunctionTraits<R(*)(Args...), void> : FunctionTraitsImpl<R, Args...> {};

    // e.g. member function pointer
    template <typename C, typename R, typename... Args>
    struct FunctionTraits<R(C::*)(Args...), void> : FunctionTraitsImpl<R, Args...> {};

    // e.g. const member function
    template <typename C, typename R, typename... Args>
    struct FunctionTraits<R(C::*)(Args...) const, void> : FunctionTraitsImpl<R, Args...> {};

    // e.g. functor/lambda
    template <typename T>
    struct FunctionTraits<T, std::void_t<decltype(&T::operator())>>
        : FunctionTraits<decltype(&T::operator())>
    {
    };

    // -----------------------------------------------------------
    // Helpers: IsSparkQuery, IsEvent, ParamTrait
    // -----------------------------------------------------------
    template <typename T>
    struct IsSparkQuery : std::false_type {};

    template <typename... Ts>
    struct IsSparkQuery<Query<Ts...>> : std::true_type {};

    template <typename T>
    struct IsEvent : std::false_type {};

    template <typename... Ts>
    struct IsEvent<Event<Ts...>> : std::true_type {};

    template <typename T>
    struct ParamTrait
    {
        using decayed = std::remove_cv_t<std::remove_reference_t<T>>;

        static constexpr bool is_query = IsSparkQuery<decayed>::value;
        static constexpr bool is_event = IsEvent<decayed>::value;
        static constexpr bool is_resource = !is_query && !is_event;
    };

    template <typename T>
    struct QueryComponents;

    template <typename... Ts>
    struct QueryComponents<Query<Ts...>>
    {
        static Query<Ts...> Get(Coordinator& coord)
        {
            return coord.template CreateQuery<Ts...>();
        }
    };

    // Helper to iterate index_sequence
    template <size_t... Is, typename F>
    void ForSequence(std::index_sequence<Is...>, F&& func)
    {
        (func(std::integral_constant<size_t, Is>{}), ...);
    }

    // -----------------------------------------------------------
    // ISystem interface (no forced_event member!)
    // -----------------------------------------------------------
    namespace detail
    {
        struct ISystem
        {
            virtual ~ISystem() = default;

            // Overload #1: no forced event
            virtual void Execute(Application& app) = 0;

            // Overload #2: with forced event pointer (but no storing in the system)
            virtual void Execute(Application& app, std::shared_ptr<IEvent> forced_event) = 0;

            // Check system's concurrency type
            virtual SystemExecutionMode GetExecutionMode() const = 0;
        };

        // We'll define the actual System<Func> below
        template <typename T>
        struct System;

        // We'll also define a new signature: 
        //   Arg ComputeArgument<Application&, std::shared_ptr<IEvent>>
        // so we can pass forced_event around.
        template <typename Arg>
        Arg ComputeArgument(Application& app, std::shared_ptr<IEvent> forced_event);
    }

    // For convenience
    template <typename T>
    using Ref = T&;

    template <typename T>
    using CRef = const T&;

    // -----------------------------------------------------------
    // The Application class
    // -----------------------------------------------------------
    class Application
    {
    public:
        Application(GraphicsApi gapi,
            const std::string& title,
            int32_t win_width,
            int32_t win_height,
            bool win_vsync = false)
            : m_mod_manager(this)
            , m_gapi(gapi)
        {
            // Start layers
            m_layer_stack.PushLayer<WindowLayer>(gapi, title, win_width, win_height, win_vsync);
            m_layer_stack.PushLayer<RendererLayer>(gapi, m_command_queue);
            m_layer_stack.PushLayer<EventLayer>(m_event_queue);

            // Register the app, coordinator, thread_pool as resources
            AddResourceRef(*this);
            AddResourceRef(m_coordinator);
            AddResourceRef(m_thread_pool);
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
            return *this;
        }

        bool IsRunning()
        {
            return GetWindowLayer().Running();
        }

        // Load mods
        Application& LoadMod(const std::string& mod_path)
        {
            auto handle = m_mod_manager.LoadLibrary(mod_path);
            if (!handle)
            {
                return *this;
            }
            auto mod_instance = m_mod_manager.LoadModInstance(handle);
            if (!mod_instance)
            {
                m_mod_manager.UnloadLibrary(handle);
            }
            return *this;
        }

        // Submit commands
        template <ValidCommand T, typename... Args>
        Application& SubmitCommand(Args&&... args)
        {
            m_command_queue.template SubmitCommand<T>(std::forward<Args>(args)...);
            return *this;
        }

        template <ValidCommand T>
        Application& SubmitCommand(T&& command)
        {
            m_command_queue.template SubmitCommand<T>(std::forward<T>(command));
            return *this;
        }

        // Submit events
        template <typename... Ts>
        Application& SubmitEvent(const Event<Ts...>& ev)
        {
            m_event_queue.SubmitEvent<Ts...>(ev);
            return *this;
        }

        template <typename T>
        Application& SubmitEvent(const T& ev)
        {
            // single-type event
            Event<T> single(ev);
            m_event_queue.SubmitEvent(single);
            return *this;
        }

        // Register multiple systems
        template <typename... Funcs>
        Application& RegisterSystems(Funcs&&... funcs)
        {
            static_assert(sizeof...(Funcs) > 0, "RegisterSystems requires at least one argument.");

            // If the last param is a SystemSettings, use it
            constexpr bool last_is_settings =
                std::is_same_v<
                std::decay_t<std::tuple_element_t<sizeof...(Funcs) - 1,
                std::tuple<Funcs...>>>,
                SystemSettings
                >;

            if constexpr (last_is_settings)
            {
                constexpr size_t num_funcs = sizeof...(Funcs) - 1;
                auto args_tuple = std::forward_as_tuple(std::forward<Funcs>(funcs)...);
                const SystemSettings& settings = std::get<num_funcs>(args_tuple);
                RegisterSystemsImpl(args_tuple, std::make_index_sequence<num_funcs>{}, settings);
            }
            else
            {
                RegisterSystems(std::forward<Funcs>(funcs)..., SystemSettings{});
            }
            return *this;
        }

        template <typename Tuple, size_t... Is>
        Application& RegisterSystemsImpl(Tuple&& tuple, std::index_sequence<Is...>, const SystemSettings& settings)
        {
            (RegisterSystem(std::get<Is>(tuple), settings), ...);
            return *this;
        }

        template <typename Func>
        Application& RegisterSystem(Func&& system_func, const SystemSettings& settings = {})
        {
            using F = std::decay_t<Func>;
            using traits = FunctionTraits<F>;
            constexpr size_t arity = traits::arity;

            // Check if any param is an event
            bool has_event_param = false;
            ForSequence(std::make_index_sequence<arity>{}, [&](auto i)
                {
                    using Arg = typename traits::template arg<i>;
                    if constexpr (ParamTrait<Arg>::is_event)
                    {
                        has_event_param = true;
                    }
                });

            // If it has an event param, we do the subscribe
            if (has_event_param)
            {
                auto system = std::make_unique<detail::System<F>>(std::forward<Func>(system_func), settings);

                // For each param, if it's an event, we subscribe
                ForSequence(std::make_index_sequence<arity>{}, [&](auto i)
                    {
                        using Arg = typename traits::template arg<i>;
                        if constexpr (ParamTrait<Arg>::is_event)
                        {
                            using RealEventT = std::decay_t<Arg>;

                            // Actually subscribe
                            SubscribeAuto<RealEventT>(m_event_queue,
                                [this, system_ptr = system.get()](RealEventT& typed_event)
                                {
                                    // We do local copy
                                    RealEventT local_copy = typed_event;

                                    // Then enqueue or call
                                    if (system_ptr->GetExecutionMode() == SystemExecutionMode::MULTITHREADED_ASYNC)
                                    {
                                        // Async
                                        m_thread_pool.Enqueue(system_ptr->m_priority,
                                            [this, system_ptr, local_copy]() mutable
                                            {
                                                // No system_ptr->m_forced_event
                                                // Instead we pass forced_event as a param
                                                auto forced_ev = std::make_shared<RealEventT>(local_copy);
                                                system_ptr->Execute(*this, forced_ev);
                                            });
                                    }
                                    else if (system_ptr->GetExecutionMode() == SystemExecutionMode::MULTITHREADED_SYNC)
                                    {
                                        // Sync
                                        auto task = m_thread_pool.Enqueue(system_ptr->m_priority,
                                            [this, system_ptr, local_copy]() mutable
                                            {
                                                auto forced_ev = std::make_shared<RealEventT>(local_copy);
                                                system_ptr->Execute(*this, forced_ev);
                                            });
                                        task.result.get(); // block
                                    }
                                    else // SINGLE_THREADED
                                    {
                                        auto forced_ev = std::make_shared<RealEventT>(local_copy);
                                        system_ptr->Execute(*this, forced_ev);
                                    }
                                });
                        }
                    });

                m_event_systems.push_back(std::move(system));
            }
            else
            {
                // Non-event system
                auto system = std::make_unique<detail::System<F>>(std::forward<Func>(system_func), settings);
                m_systems[settings.phase].push_back(std::move(system));
            }
            return *this;
        }

        // Resource management
        template <typename T>
        Application& AddResource(T&& resource)
            requires (!std::is_pointer_v<T>)
        {
            m_resources[std::type_index(typeid(T))] =
                threading::AutoLockable<std::decay_t<T>>(std::forward<T>(resource));
            return *this;
        }

        template <typename T>
        Application& AddResource(T resource)
            requires (std::is_pointer_v<T>)
        {
            m_resources[std::type_index(typeid(T))] = resource;
            return *this;
        }

        template <typename T>
        bool HasResource() const
        {
            return (m_resources.find(std::type_index(typeid(T))) != m_resources.end());
        }

        // For non-pointer T
        template <typename T>
            requires (!std::is_pointer_v<T>)
        threading::AutoLockable<T>& GetResource()
        {
            auto it = m_resources.find(std::type_index(typeid(T)));
            if (it == m_resources.end())
            {
                throw std::runtime_error("Requested resource not found!");
            }
            return std::any_cast<threading::AutoLockable<T>&>(it->second);
        }

        // For pointer T
        template <typename T>
            requires (std::is_pointer_v<T>)
        threading::AutoLockable<T>& GetResource()
        {
            auto it = m_resources.find(std::type_index(typeid(T)));
            if (it == m_resources.end())
            {
                throw std::runtime_error("Requested resource not found!");
            }
            // We stored it as a shared_ptr<threading::AutoLockable<T>>
            return *std::any_cast<std::shared_ptr<threading::AutoLockable<T>>>(it->second);
        }

        template <typename T>
        Application& AddResourceRef(T& resource)
        {
            // store under typeid(T*)
            m_resources[std::type_index(typeid(T*))] =
                std::make_shared<threading::AutoLockable<T*>>(&resource);
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

        // Dispatch systems for a given phase
        void DispatchSystemsForPhase(SystemPhase phase)
        {
            auto it = m_systems.find(phase);
            if (it == m_systems.end()) return;

            std::vector<std::future<void>> sync_tasks;
            for (auto& system_ptr : it->second)
            {
                switch (system_ptr->GetExecutionMode())
                {
                case SystemExecutionMode::SINGLE_THREADED:
                    system_ptr->Execute(*this);
                    break;
                case SystemExecutionMode::MULTITHREADED_SYNC:
                {
                    // Enqueue, wait
                    auto task = m_thread_pool.Enqueue(SystemPriority::NORMAL,
                        [this, &system_ptr]()
                        {
                            system_ptr->Execute(*this);
                        });
                    sync_tasks.emplace_back(std::move(task.result));
                    break;
                }
                case SystemExecutionMode::MULTITHREADED_ASYNC:
                {
                    // Fire-and-forget
                    m_thread_pool.Enqueue(SystemPriority::NORMAL,
                        [this, &system_ptr]()
                        {
                            system_ptr->Execute(*this);
                        });
                    break;
                }
                }
            }
            // Join all sync tasks
            for (auto& fut : sync_tasks)
            {
                fut.get();
            }
        }

        IRenderer& GetRenderer()
        {
            return GetRendererLayer().GetRenderer();
        }

        Application& ChangeGraphicsApi(GraphicsApi gapi)
        {
            if (gapi == m_gapi) return *this;
            GetWindowLayer().SetGraphicsApi(gapi);
            GetRendererLayer().SetGraphicsApi(gapi);
            return *this;
        }

    private:
        // for multi-event subscription
        template <typename E>
        struct EventExtractor;

        template <typename... Ts>
        struct EventExtractor<Event<Ts...>>
        {
            using pack = std::tuple<Ts...>;
        };

        template <typename E, typename F>
        void SubscribeAuto(EventQueue& eq, F&& user_callback)
        {
            using pack_t = typename EventExtractor<E>::pack;
            SubscribeAutoImpl<E>(
                eq,
                std::forward<F>(user_callback),
                std::make_index_sequence<std::tuple_size_v<pack_t>>{}
            );
        }

        template <typename E, typename F, size_t... Is>
        void SubscribeAutoImpl(EventQueue& eq, F&& user_callback, std::index_sequence<Is...>)
        {
            using pack_t = typename EventExtractor<E>::pack;

            // eq.Subscribe<Ts...>()
            eq.template Subscribe<std::tuple_element_t<Is, pack_t>...>(
                [cb = std::forward<F>(user_callback)](E& ev) mutable
                {
                    // call user callback with E& 
                    cb(ev);
                }
            );
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
        LayerStack m_layer_stack;
        CommandQueue m_command_queue;
        EventQueue m_event_queue;
        ModManager m_mod_manager;
        threading::ThreadPool m_thread_pool;

        GraphicsApi m_gapi;
        DeltaTime<double> m_dt;

        Coordinator m_coordinator;

        // Systems
        std::unordered_map<SystemPhase, std::vector<std::unique_ptr<detail::ISystem>>> m_systems;
        std::vector<std::unique_ptr<detail::ISystem>> m_event_systems;

        // Resource store
        std::unordered_map<std::type_index, std::any> m_resources;

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;
    };

    // -----------------------------------------------------------
    // detail::System<Func> Implementation (no forced_event member)
    // -----------------------------------------------------------
    namespace detail
    {
        template <typename Func>
        struct System : ISystem
        {
            Func m_func;
            SystemExecutionMode m_execution_mode;
            SystemPriority m_priority;

            // NOTE: no m_forced_event stored here

            System(Func&& f, const SystemSettings& settings)
                : m_func(std::forward<Func>(f))
                , m_execution_mode(settings.execution_mode)
                , m_priority(settings.priority)
            {
            }

            void Execute(Application& app) override
            {
                // No forced event
                ExecuteImpl(app, nullptr);
            }

            void Execute(Application& app, std::shared_ptr<IEvent> forced_event) override
            {
                // We pass forced_event along as a parameter
                ExecuteImpl(app, forced_event);
            }

            SystemExecutionMode GetExecutionMode() const override
            {
                return m_execution_mode;
            }

        private:
            template <size_t... I>
            void CallFunc(Application& app, std::shared_ptr<IEvent> forced_evt, std::index_sequence<I...>)
            {
                using traits = FunctionTraits<Func>;

                // We expand the system function's parameter pack
                // calling ComputeArgument<Arg> with (app, forced_evt)
                m_func(
                    ComputeArgument<typename traits::template arg<I>>(app, forced_evt)...
                );
            }

            void ExecuteImpl(Application& app, std::shared_ptr<IEvent> forced_evt)
            {
                using traits = FunctionTraits<Func>;
                constexpr size_t arity = traits::arity;

                // expand indices
                CallFunc(app, forced_evt, std::make_index_sequence<arity>{});
            }
        };

        // -----------------------------------------------------------
        // If Arg is an event, we do a dynamic_pointer_cast
        // If not, we do the normal resource / query / etc.
        // -----------------------------------------------------------
        template <typename Arg>
        Arg ComputeArgumentImpl(Application& app, std::shared_ptr<IEvent> forced_evt, std::true_type /* is_event */)
        {
            // We want Arg = e.g. Event<Blah,Acceleration>
            if (!forced_evt)
            {
                throw std::runtime_error("System expects an event, but no forced event was provided!");
            }

            using ArgValue = std::remove_reference_t<Arg>;
            auto typed_event = std::dynamic_pointer_cast<ArgValue>(forced_evt);
            if (!typed_event)
            {
                throw std::runtime_error("Could not cast forced event to the expected multi-event type!");
            }
            return *typed_event;
        }

        template <typename Arg>
        Arg ComputeArgumentImpl(Application& app, std::shared_ptr<IEvent> /* forced_evt */, std::false_type /* is_event */)
        {
            // If query
            if constexpr (ParamTrait<Arg>::is_query)
            {
                return QueryComponents<Arg>::Get(app.GetCoordinator());
            }
            // If resource
            else if constexpr (ParamTrait<Arg>::is_resource)
            {
                using bare_t = std::remove_reference_t<Arg>;
                if (app.HasResource<bare_t>())
                {
                    auto& auto_lockable = app.GetResource<bare_t>();
                    if constexpr (std::is_const_v<bare_t>)
                    {
                        return auto_lockable.CLock();
                    }
                    else
                    {
                        return auto_lockable.Lock();
                    }
                }
                else if (app.HasResource<bare_t*>())
                {
                    auto& auto_lockable = app.GetResource<bare_t*>();
                    if constexpr (std::is_const_v<bare_t>)
                    {
                        return auto_lockable.CLock();
                    }
                    else
                    {
                        return auto_lockable.Lock();
                    }
                }
                else
                {
                    throw std::runtime_error("Dependency must be registered as a resource for automatic locking!");
                }
            }
            else
            {
                static_assert(sizeof(Arg) == 0, "Unhandled Arg type in ComputeArgument!");
            }
        }

        // The public function
        template <typename Arg>
        Arg ComputeArgument(Application& app, std::shared_ptr<IEvent> forced_evt)
        {
            constexpr bool is_event = ParamTrait<Arg>::is_event;
            return ComputeArgumentImpl<Arg>(
                app,
                forced_evt,
                std::integral_constant<bool, is_event>{}
            );
        }
    } // namespace detail

} // namespace spark

#endif // SPARK_APPLICATION_HPP
