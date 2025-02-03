#ifndef SPARK_APPLICATION_HPP
#define SPARK_APPLICATION_HPP

#include "spark_pch.hpp"
#include "spark_ecs.hpp"
#include "spark_window.hpp"
#include "spark_event.hpp"
#include "spark_threading.hpp"  // Must define AutoLockable, LockedRef, LockedCRef, ThreadPool, etc.
#include "spark_stopwatch.hpp"
#include "spark_graphics_api.hpp"
#include "spark_defer.hpp"
#include "spark_layer_stack.hpp"
#include "spark_renderer_layer.hpp"
#include "spark_command_queue.hpp"
#include "spark_event_layer.hpp"
#include "special/spark_modding.hpp"

#include <any>
#include <functional>
#include <memory>
#include <stdexcept>
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <tuple>
#include <vector>
#include <string>
#include <future>

namespace spark
{
    class Application;

    //================================================================
    // System-related enums and settings
    //================================================================
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

    // All settings for registering a system.
    struct SystemSettings
    {
        SystemPhase phase = SystemPhase::UPDATE;
        SystemExecutionMode execution_mode = SystemExecutionMode::SINGLE_THREADED;
        SystemPriority priority = SystemPriority::NORMAL;
    };

    //================================================================
    // FunctionTraits (unchanged)
    //================================================================
    template<typename R, typename... Args>
    struct FunctionTraitsImpl
    {
        using ReturnType = R;
        using ArgsTuple = std::tuple<Args...>;
        static constexpr size_t arity = sizeof...(Args);

        template <size_t Index>
        using Arg = std::tuple_element_t<Index, ArgsTuple>;
    };

    template<typename T, typename = void>
    struct FunctionTraits;

    template<typename R, typename... Args>
    struct FunctionTraits<R(Args...), void> : FunctionTraitsImpl<R, Args...> {};

    template<typename R, typename... Args>
    struct FunctionTraits<R(*)(Args...), void> : FunctionTraits<R(Args...), void> {};

    template<typename C, typename R, typename... Args>
    struct FunctionTraits<R(C::*)(Args...), void> : FunctionTraits<R(Args...), void> {};

    template<typename C, typename R, typename... Args>
    struct FunctionTraits<R(C::*)(Args...) const, void> : FunctionTraits<R(Args...), void> {};

    template<typename T>
    struct FunctionTraits<T, std::void_t<decltype(&T::operator())>>
        : FunctionTraits<decltype(&T::operator())> {};

    //================================================================
    // Type traits for dependency injection
    //================================================================
    template<typename T>
    struct IsSparkQuery : std::false_type {};

    template<typename... Ts>
    struct IsSparkQuery<Query<Ts...>> : std::true_type {};

    template<typename T>
    struct IsEvent : std::false_type {};

    template<typename... Ts>
    struct IsEvent<Event<Ts...>> : std::true_type {};

    template<typename T>
    struct ParamTrait
    {
        using Decayed = std::remove_cv_t<std::remove_reference_t<T>>;

        static constexpr bool is_application = std::is_same_v<Decayed, Application>;
        static constexpr bool is_coordinator = std::is_same_v<Decayed, Coordinator>;
        static constexpr bool is_thread_pool = std::is_same_v<Decayed, threading::ThreadPool>;
        static constexpr bool is_query = IsSparkQuery<Decayed>::value;
        static constexpr bool is_event = IsEvent<Decayed>::value;
        static constexpr bool is_resource = !is_query;
    };

    template<typename T>
    struct QueryComponents;

    template<typename... Ts>
    struct QueryComponents<Query<Ts...>>
    {
        static Query<Ts...> Get(Coordinator& coord)
        {
            return coord.template CreateQuery<Ts...>();
        }
    };

    template <size_t... Is, typename F>
    void ForSequence(std::index_sequence<Is...>, F&& f)
    {
        (f(std::integral_constant<size_t, Is>{}), ...);
    }

    //================================================================
    // Dependency Injection Helpers in namespace detail
    //================================================================
    namespace detail
    {
        template<typename Arg>
        auto ComputeArgument(Application& app) -> decltype(auto);

        template<typename Arg>
        auto ComputeArgument(Application& app, std::true_type /* is_event */) -> decltype(auto);

        template<typename Arg>
        auto ComputeArgument(Application& app, std::false_type /* is_event */) -> decltype(auto);

        //================================================================
        // System invocation wrappers.
        //================================================================
        struct ISystem
        {
            virtual ~ISystem() = default;
            virtual void Execute(Application& app) = 0;
            virtual SystemExecutionMode get_execution_mode() const = 0;
        };

        template <typename Func>
        struct System : ISystem
        {
            Func m_func;
            SystemExecutionMode m_execution_mode;
            SystemPriority m_priority;

            System(Func&& f,
                const SystemSettings& settings = { SystemPhase::UPDATE, SystemExecutionMode::SINGLE_THREADED, SystemPriority::NORMAL })
                : m_func(std::forward<Func>(f))
                , m_execution_mode(settings.execution_mode)
                , m_priority(settings.priority)
            {
            }

            virtual void Execute(Application& app) override
            {
                using Traits = FunctionTraits<Func>;
                constexpr size_t arity = Traits::arity;
                ExecuteImpl(app, std::make_index_sequence<arity>{});
            }

            virtual SystemExecutionMode get_execution_mode() const override
            {
                return m_execution_mode;
            }

            SystemPriority get_priority() const
            {
                return m_priority;
            }

        private:
            template <size_t... I>
            void ExecuteImpl(Application& app, std::index_sequence<I...>)
            {
                using Traits = FunctionTraits<Func>;
                m_func(ComputeArgument<typename Traits::template Arg<I>>(app)...);
            }
        };
    } // namespace detail

    //================================================================
    // Convenience aliases
    //================================================================
    template <typename T>
    using Ref = T&;

    template <typename T>
    using CRef = const T&;

    //================================================================
    // The Application class
    //================================================================
    class Application
    {
    public:
        Application(GraphicsApi gapi,
            const std::string& title,
            i32 win_width,
            i32 win_height,
            bool win_vsync = false)
            : m_mod_manager(this)
            , m_gapi(gapi)
        {
            m_layer_stack.PushLayer<WindowLayer>(gapi, title, win_width, win_height, win_vsync);
            m_layer_stack.PushLayer<RendererLayer>(gapi, m_command_queue);
            m_layer_stack.PushLayer<EventLayer>(m_event_queue);

            // Register core dependencies as resources.
            AddResourceRef(*this);
            AddResourceRef(m_coordinator);
            AddResourceRef(m_thread_pool);
        }

        ~Application()
        {
            Close();
        }

        Application& AddThreads(usize num_threads)
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

        Application& LoadMod(const std::string& mod_path)
        {
            auto handle = m_mod_manager.LoadLibrary(mod_path);
            if (!handle) return *this;
            auto mod_instance = m_mod_manager.LoadModInstance(handle);
            if (!mod_instance)
            {
                m_mod_manager.UnloadLibrary(handle);
                return *this;
            }
            return *this;
        }

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

        template <typename... Ts>
        Application& SubmitEvent(const Event<Ts...>& ev)
        {
            m_event_queue.SubmitEvent<Ts...>(ev);
            return *this;
        }

        template <typename T, typename... Ts>
        Application& SubmitEvent(const T& ev)
        {
            Event<T, Ts...> event(ev);
            m_event_queue.SubmitEvent<T, Ts...>(event);
            return *this;
        }

        // For events, we expect m_current_event to store a std::shared_ptr<T>
        template<typename T>
        T& GetCurrentEvent()
        {
            return *std::any_cast<std::shared_ptr<T>>(m_current_event);
        }

        void SetCurrentEvent(std::any event)
        {
            m_current_event = std::move(event);
        }

        void ClearCurrentEvent()
        {
            m_current_event.reset();
        }

        // RegisterSystem accepts a SystemSettings struct.
        template<typename Func>
        Application& RegisterSystem(Func&& system_func,
            const SystemSettings& settings = {})
        {
            using F = std::decay_t<Func>;
            using Traits = FunctionTraits<F>;
            constexpr usize arity = Traits::arity;

            bool has_event_param = false;
            ForSequence(std::make_index_sequence<arity>{}, [&](auto i) {
                using Arg = typename Traits::template Arg<decltype(i)::value>;
                if constexpr (ParamTrait<Arg>::is_event)
                {
                    has_event_param = true;
                }
                });

            if (has_event_param)
            {
                auto system = std::make_unique<detail::System<F>>(
                    std::forward<Func>(system_func), settings);
                // For event systems, subscribe to events.
                // Inside RegisterSystem, in the event branch (for each event parameter)
                ForSequence(std::make_index_sequence<arity>{}, [&](auto i) {
                    using Arg = typename Traits::template Arg<decltype(i)::value>;
                    if constexpr (ParamTrait<Arg>::is_event)
                    {
                        using EventT = std::decay_t<Arg>;
                        // We capture the raw pointer from the unique_ptr by calling system.get()
                        [this, system_ptr = system.get()] <typename... Us>(Event<Us...>*)
                        {
                            ([this, system_ptr]() {
                                m_event_queue.template Subscribe<Us>(
                                    [this, system_ptr](Event<Us>& event)
                                    {
                                        // Create a shared_ptr copy of the submitted event.
                                        auto raw_event_ptr = std::make_shared<Event<Us>>(event);
                                        // Construct the expected event type from the submitted event.
                                        // This uses the conversion constructor in Event.
                                        std::shared_ptr<EventT> event_ptr = std::make_shared<EventT>(*raw_event_ptr);
                                        // If the conversion fails (which it shouldn't if your conversion constructor is correct),
                                        // throw an exception.
                                        if (!event_ptr)
                                        {
                                            throw std::runtime_error("Event type mismatch in subscription. Make sure the event submitted matches the event type expected by the system.");
                                        }

                                        if (system_ptr->get_execution_mode() == SystemExecutionMode::MULTITHREADED_ASYNC)
                                        {
                                            m_thread_pool.Enqueue(system_ptr->get_priority(),
                                                [system_ptr, this, event_ptr]() mutable
                                                {
                                                    SetCurrentEvent(event_ptr);
                                                    system_ptr->Execute(*this);
                                                    ClearCurrentEvent();
                                                });
                                        }
                                        else if (system_ptr->get_execution_mode() == SystemExecutionMode::MULTITHREADED_SYNC)
                                        {
                                            auto task = m_thread_pool.Enqueue(system_ptr->get_priority(),
                                                [system_ptr, this, event_ptr]() mutable
                                                {
                                                    SetCurrentEvent(event_ptr);
                                                    system_ptr->Execute(*this);
                                                    ClearCurrentEvent();
                                                });
                                            task.result.get();
                                        }
                                        else // SINGLE_THREADED
                                        {
                                            SetCurrentEvent(event_ptr);
                                            system_ptr->Execute(*this);
                                            ClearCurrentEvent();
                                        }
                                    }
                                );
                                }(), ...);
                        }(static_cast<EventT*>(nullptr));
                    }
                    });

                m_event_systems.push_back(std::move(system));
            }
            else
            {
                auto system = std::make_unique<detail::System<F>>(
                    std::forward<Func>(system_func), settings);
                m_systems[settings.phase].push_back(std::move(system));
            }
            return *this;
        }

        Application& SetDeltaTime(f32 dt)
        {
            m_dt = dt;
            return *this;
        }

        bool IsRunning()
        {
            return GetWindowLayer().Running();
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

        Coordinator& GetCoordinator()
        {
            return m_coordinator;
        }

        threading::ThreadPool& GetThreadPool()
        {
            return m_thread_pool;
        }

        //------------------------------------------------------------------
        // AddResource and GetResource (unchanged)
        //------------------------------------------------------------------
        template <typename T>
        std::enable_if_t<!std::is_pointer_v<T>, Application&>
            AddResource(T&& resource)
        {
            m_resources[std::type_index(typeid(T))] =
                threading::AutoLockable<std::decay_t<T>>(std::forward<T>(resource));
            return *this;
        }

        template <typename T>
        std::enable_if_t<std::is_pointer_v<T>, Application&>
            AddResource(T resource)
        {
            m_resources[std::type_index(typeid(T))] = resource;
            return *this;
        }

        template <typename T>
        bool HasResource() const
        {
            return m_resources.find(std::type_index(typeid(T))) != m_resources.end();
        }

        template <typename T, typename = std::enable_if_t<!std::is_pointer_v<T>>>
        threading::AutoLockable<T>& GetResource()
        {
            auto it = m_resources.find(std::type_index(typeid(T)));
            if (it == m_resources.end())
                throw std::runtime_error("Requested resource not found!");
            return std::any_cast<threading::AutoLockable<T>&>(it->second);
        }

        // New pointer version – returns a reference to the AutoLockable wrapper
        template <typename T>
        std::enable_if_t<std::is_pointer_v<T>, threading::AutoLockable<T>&> GetResource()
        {
            auto it = m_resources.find(std::type_index(typeid(T)));
            if (it == m_resources.end())
                throw std::runtime_error("Requested resource not found!");
            return *std::any_cast<std::shared_ptr<threading::AutoLockable<T>>>(it->second);
        }

        template <typename T>
        Application& AddResourceRef(T& resource)
        {
            // Store the resource under key typeid(T*) so that the pointer branch in ComputeArgument works correctly.
            m_resources[std::type_index(typeid(T*))] =
                std::make_shared<threading::AutoLockable<T*>>(threading::AutoLockable<T*>(&resource));
            return *this;
        }




    private:
        // For non-event systems, accumulate MULTITHREADED_SYNC tasks.
        void DispatchSystemsForPhase(SystemPhase phase)
        {
            std::vector<std::future<void>> sync_tasks;
            auto it = m_systems.find(phase);
            if (it != m_systems.end())
            {
                for (auto& system_ptr : it->second)
                {
                    switch (system_ptr->get_execution_mode())
                    {
                    case SystemExecutionMode::SINGLE_THREADED:
                        system_ptr->Execute(*this);
                        break;
                    case SystemExecutionMode::MULTITHREADED_SYNC:
                    {
                        auto task = m_thread_pool.Enqueue(SystemPriority::NORMAL,
                            [raw = system_ptr.get(), this]() {
                                raw->Execute(*this);
                            });
                        sync_tasks.push_back(std::move(task.result));
                        break;
                    }
                    case SystemExecutionMode::MULTITHREADED_ASYNC:
                        m_thread_pool.Enqueue(SystemPriority::NORMAL,
                            [raw = system_ptr.get(), this]() {
                                raw->Execute(*this);
                            });
                        break;
                    }
                }
                for (auto& fut : sync_tasks)
                {
                    fut.get();
                }
            }
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
        f32 m_dt = 0.0f;

        std::vector<std::unique_ptr<detail::ISystem>> m_event_systems;
        std::any m_current_event;

        Coordinator m_coordinator;

        std::unordered_map<std::type_index, std::any> m_resources;
        std::unordered_map<SystemPhase, std::vector<std::unique_ptr<detail::ISystem>>> m_systems;

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;
    };

    //================================================================
    // ResourceGuard (unchanged)
    //================================================================
    template <typename T>
    class ResourceGuard
    {
    public:
        ResourceGuard(threading::LockedRef<T>&& locked_ref)
            : m_locked_ref(std::make_optional<threading::LockedRef<T>>(std::move(locked_ref)))
            , m_ptr(&m_locked_ref->Get())
        {
        }

        ResourceGuard(threading::LockedCRef<T>&& locked_cref)
            : m_locked_cref(std::make_optional<threading::LockedCRef<T>>(std::move(locked_cref)))
            , m_ptr(&m_locked_cref->Get())
        {
        }

        ResourceGuard(T& ref)
            : m_ptr(&ref)
        {
        }

        operator T& () { return *m_ptr; }
        T& Get() { return *m_ptr; }

    private:
        std::optional<threading::LockedRef<T>> m_locked_ref;
        std::optional<threading::LockedCRef<T>> m_locked_cref;
        T* m_ptr;
    };

    //================================================================
    // ComputeArgument implementations.
    //================================================================
    namespace detail
    {
        template<typename Arg>
        auto ComputeArgument(Application& app) -> decltype(auto)
        {
            constexpr bool is_event = ParamTrait<Arg>::is_event;
            return ComputeArgument<Arg>(app, std::integral_constant<bool, is_event>{});
        }

        template<typename Arg>
        auto ComputeArgument(Application& app, std::true_type /* is_event */) -> decltype(auto)
        {
            return app.GetCurrentEvent<Arg>();
        }

        template<typename Arg>
        auto ComputeArgument(Application& app, std::false_type /* is_event */) -> decltype(auto)
        {
            if constexpr (ParamTrait<Arg>::is_query)
            {
                return QueryComponents<Arg>::Get(app.GetCoordinator());
            }
            else if constexpr (ParamTrait<Arg>::is_resource)
            {
                static_assert(std::is_lvalue_reference_v<Arg>,
                    "Automatic locking requires an lvalue reference parameter type.");

                using bare_t = std::remove_reference_t<Arg>;
                if (app.HasResource<bare_t>())
                {
                    auto& auto_lockable = app.GetResource<bare_t>();
                    if constexpr (std::is_const_v<std::remove_reference_t<Arg>>)
                    {
                        return ResourceGuard<bare_t>(auto_lockable.CLock());
                    }
                    else
                    {
                        return ResourceGuard<bare_t>(auto_lockable.Lock());
                    }
                }
                else if (app.HasResource<bare_t*>())
                {
                    // Now auto_lockable for pointer types is of type AutoLockable<bare_t*>
                    auto& auto_lockable = app.GetResource<bare_t*>();
                    if constexpr (std::is_const_v<std::remove_reference_t<Arg>>)
                    {
                        return ResourceGuard<bare_t>(auto_lockable.CLock());
                    }
                    else
                    {
                        return ResourceGuard<bare_t>(auto_lockable.Lock());
                    }
                }
                else
                {
                    throw std::runtime_error("Dependency must be registered as a resource (using AddResource or AddResourceRef) for automatic locking.");
                }
            }
            else
            {
                static_assert(sizeof(Arg) == 0, "Unhandled Arg type in ComputeArgument.");
            }
        }
    }
} // namespace spark

#endif // SPARK_APPLICATION_HPP
