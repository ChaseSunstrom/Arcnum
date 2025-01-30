#ifndef SPARK_APPLICATION_HPP
#define SPARK_APPLICATION_HPP

#include "spark_pch.hpp"
#include "spark_ecs.hpp"
#include "spark_window.hpp"
#include "spark_event.hpp"
#include "spark_stopwatch.hpp"
#include "spark_graphics_api.hpp"
#include "spark_defer.hpp"
#include "spark_layer_stack.hpp"
#include "spark_renderer_layer.hpp"
#include "spark_command_queue.hpp"
#include "spark_event_layer.hpp"
#include "special/spark_modding.hpp"

namespace spark
{
    class Application;

    enum class LifecyclePhase
    {
        ON_START,
        BEFORE_UPDATE,
        UPDATE,
        AFTER_UPDATE,
        ON_SHUTDOWN
    };

    // FunctionTraits implementation
    template<typename R, typename... Args>
    struct FunctionTraitsImpl
    {
        using ReturnType = R;
        using ArgsTuple = std::tuple<Args...>;
        static constexpr size_t arity = sizeof...(Args);

        template <size_t Index>
        using Arg = std::tuple_element_t<Index, ArgsTuple>;
    };

    // Primary template
    template<typename T, typename = void>
    struct FunctionTraits;

    // Specialization for function types
    template<typename R, typename... Args>
    struct FunctionTraits<R(Args...), void> : FunctionTraitsImpl<R, Args...> {};

    // Specialization for function pointers
    template<typename R, typename... Args>
    struct FunctionTraits<R(*)(Args...), void> : FunctionTraits<R(Args...), void> {};

    // Specialization for member function pointers
    template<typename C, typename R, typename... Args>
    struct FunctionTraits<R(C::*)(Args...), void> : FunctionTraits<R(Args...), void> {};

    // Specialization for const member function pointers
    template<typename C, typename R, typename... Args>
    struct FunctionTraits<R(C::*)(Args...) const, void> : FunctionTraits<R(Args...), void> {};

    // Specialization for functors (objects with operator())
    template<typename T>
    struct FunctionTraits<T, std::void_t<decltype(&T::operator())>>
        : FunctionTraits<decltype(&T::operator())> {};

    // Helper to determine if a type is a Query
    template<typename T>
    struct IsSparkQuery : std::false_type {};

    template<typename... Ts>
    struct IsSparkQuery<Query<Ts...>> : std::true_type {};

    // Helper to check if a type is an Event
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
        static constexpr bool is_query = IsSparkQuery<Decayed>::value;
        static constexpr bool is_event = IsEvent<Decayed>::value;
        static constexpr bool is_resource = !is_application && !is_coordinator && !is_query && !is_event;
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
    void for_sequence(std::index_sequence<Is...>, F&& f) {
        (f(std::integral_constant<size_t, Is>{}), ...);
    }

    // Removed the faulty specialization:
    // template<typename... Ts>
    // struct ParamTrait<Query<Ts...>&> : ParamTrait<Query<Ts...>> {};

    namespace detail
    {
        // Primary ComputeArgument that selects the correct overload
        template<typename Arg>
        Arg ComputeArgument(Application& app);
        // Overload for non-Query types
        template<typename Arg>
        Arg ComputeArgument(Application& app, std::false_type /* is_query */);

        // Overload for Query types
        template<typename Arg>
        Arg ComputeArgument(Application& app, std::true_type /* is_query */);

        // Base class for type-erased systems
        struct ISystem
        {
            virtual ~ISystem() = default;
            virtual void Execute(Application& app) = 0;
        };

        // Templated derived class for systems
        template<typename Func>
        struct System : ISystem
        {
            Func func;

            System(Func&& f) : func(std::forward<Func>(f)) {}

            virtual void Execute(Application& app) override;
        private:
            template<size_t... I>
            void ExecuteImpl(Application& app, std::index_sequence<I...>);
        };

    }

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
            // If there's only one app, we can do:
            if (!s_app_ptr)
            {
                s_app_ptr = this;
            }

            m_layer_stack.PushLayer<WindowLayer>(gapi, title, win_width, win_height, win_vsync);
            m_layer_stack.PushLayer<RendererLayer>(gapi, m_command_queue);
            m_layer_stack.PushLayer<EventLayer>(m_event_queue);
        }

        template<typename T>
        Application& AddResource(T&& resource)
        {
            m_resources[std::type_index(typeid(T))] = std::forward<T>(resource);
            return *this;
        }

        template<typename T>
        T& GetResource()
        {
            auto it = m_resources.find(std::type_index(typeid(T)));
            if (it == m_resources.end())
            {
                throw std::runtime_error("Requested resource not found!");
            }
            return std::any_cast<T&>(it->second);
        }

        Application& Start()
        {
            // ON_START
            for (auto& system : m_systems[LifecyclePhase::ON_START])
            {
                system->Execute(*this);
            }
            m_layer_stack.Start();
            return *this;
        }

        Application& Run()
        {
            while (IsRunning())
            {
                // BEFORE_UPDATE
                for (auto& system : m_systems[LifecyclePhase::BEFORE_UPDATE])
                {
                    system->Execute(*this);
                }

                m_layer_stack.Update(m_dt);

                // UPDATE
                for (auto& system : m_systems[LifecyclePhase::UPDATE])
                {
                    system->Execute(*this);
                }

                // AFTER_UPDATE
                for (auto& system : m_systems[LifecyclePhase::AFTER_UPDATE])
                {
                    system->Execute(*this);
                }
            }
            return *this;
        }

        Application& Close()
        {
            // ON_SHUTDOWN
            for (auto& system : m_systems[LifecyclePhase::ON_SHUTDOWN])
            {
                system->Execute(*this);
            }
            m_layer_stack.Stop();
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
            // Construct an Event<T, Ts...> from 'ev'
            // => your Event partial specialization must have a constructor
            //    that takes (const T&) or T (some data type).
            Event<T, Ts...> event(ev);

            // Then pass it to the queue as an Event<T, Ts...>
            m_event_queue.SubmitEvent<T, Ts...>(event);

            return *this;
        }

        template<typename T>
        T& GetCurrentEvent() {
            return std::any_cast<std::reference_wrapper<T>>(m_current_event).get();
        }

        void SetCurrentEvent(std::any event) {
            m_current_event = std::move(event);
        }

        void ClearCurrentEvent() {
            m_current_event.reset();
        }
        
        template<typename Func>
        Application& RegisterSystem(Func&& system_func,
            LifecyclePhase phase = LifecyclePhase::UPDATE)
        {
            using F = std::decay_t<Func>;
            using Traits = FunctionTraits<F>;
            constexpr size_t arity = Traits::arity;

            bool has_event_param = false;
            for_sequence(std::make_index_sequence<arity>{}, [&](auto I) {
                using Arg = typename Traits::template Arg<decltype(I)::value>;
                if constexpr (ParamTrait<Arg>::is_event) {
                    has_event_param = true;
                }
                });

            if (has_event_param) {
                auto system = std::make_unique<detail::System<F>>(std::forward<Func>(system_func));

                for_sequence(std::make_index_sequence<arity>{}, [&](auto I) {
                    using Arg = typename Traits::template Arg<decltype(I)::value>;
                    if constexpr (ParamTrait<Arg>::is_event) {
                        using EventT = std::decay_t<Arg>;
                        [this, system_ptr = system.get()] <typename... Us>(Event<Us...>*) {
                            ([this, system_ptr] {
                                // Subscribe to Event<Us> type (not raw Us)
                                m_event_queue.template Subscribe<Us>(
                                    // Correct lambda parameter to Event<Us>&
                                    [this, system_ptr](Event<Us>& event) {
                                        // Convert Event<Us> to the system's expected EventT
                                        EventT wrapped_event(event);
                                        SetCurrentEvent(std::ref(wrapped_event));
                                        system_ptr->Execute(*this);
                                        ClearCurrentEvent();
                                    }
                                );
                                }(), ...);
                        }(static_cast<EventT*>(nullptr));
                    }
                    });

                m_event_systems.push_back(std::move(system));
            }
            else {
                auto system = std::make_unique<detail::System<F>>(std::forward<Func>(system_func));
                m_systems[phase].push_back(std::move(system));
            }

            return *this;
        }

        Application& SetDeltaTime(float dt)
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
            if (gapi == m_gapi)
            {
                return *this;
            }
            GetWindowLayer().SetGraphicsApi(gapi);
            GetRendererLayer().SetGraphicsApi(gapi);
            return *this;
        }

        Coordinator& GetCoordinator()
        {
            return m_coordinator;
        }

    private:
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
        // snake_case for member variables
        LayerStack m_layer_stack;
        CommandQueue m_command_queue;
        EventQueue m_event_queue;
        ModManager m_mod_manager;
        GraphicsApi m_gapi;
        f32 m_dt = 0.0f;


        std::vector<std::unique_ptr<detail::ISystem>> m_event_systems;
        std::any m_current_event;

        Coordinator m_coordinator;

        // Resource storage
        std::unordered_map<std::type_index, std::any> m_resources;

        // Systems by phase: each system is stored as unique_ptr<ISystem>
        std::unordered_map<
            LifecyclePhase,
            std::vector<std::unique_ptr<detail::ISystem>>>
            m_systems;

        static inline Application* s_app_ptr = nullptr;

    public:
        // Delete copy and move constructors/assignments
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        ~Application()
        {
            if (s_app_ptr == this)
            {
                s_app_ptr = nullptr;
            }
        }

        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;
    };

    namespace detail
    {
        template<typename Arg>
        Arg ComputeArgument(Application& app, std::true_type /* is_event */)
        {
            return app.GetCurrentEvent<Arg>();
        }

        template<typename Arg>
        Arg ComputeArgument(Application& app, std::false_type /* is_event */)
        {
            using Trait = ParamTrait<Arg>;

            if constexpr (Trait::is_application) {
                return app;
            }
            else if constexpr (Trait::is_coordinator) {
                return app.GetCoordinator();
            }
            else if constexpr (Trait::is_resource) {
                return app.GetResource<Arg>();
            }
            else if constexpr (Trait::is_query) {
                return QueryComponents<Arg>::Get(app.GetCoordinator());
            }
            else {
                static_assert(sizeof(Arg) == 0, "Unhandled Arg type in ComputeArgument.");
            }
        }

        template<typename Arg>
        Arg ComputeArgument(Application& app)
        {
            constexpr bool is_event = ParamTrait<Arg>::is_event;
            return ComputeArgument<Arg>(app, std::integral_constant<bool, is_event>{});
        }


        template<typename Func>
        void System<Func>::Execute(Application& app)
        {
            using Traits = FunctionTraits<Func>;
            constexpr size_t arity = Traits::arity;

            // Collect dependencies
            ExecuteImpl(app, std::make_index_sequence<arity>{});
        }

        template<typename Func>
    	template<size_t... I>
        void System<Func>::ExecuteImpl(Application& app, std::index_sequence<I...>)
        {
            using Traits = FunctionTraits<Func>;

            // Inject dependencies based on parameter types
            func(ComputeArgument<typename Traits::template Arg<I>>(app)...);
        }
    }

    template <typename T>
    using Ref = T&;

    template <typename T>
    using CRef = const T&;


} // namespace spark

#endif // SPARK_APPLICATION_HPP
