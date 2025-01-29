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

    template<typename R, typename... Args>
    struct FunctionTraitsImpl
    {
        static constexpr size_t arg_count = sizeof...(Args);

        using ReturnType = R;

        template<size_t Index>
        using ArgType = std::tuple_element_t<Index, std::tuple<Args...>>;
    };

    // Primary template
    template<typename T, typename = void>
    struct FunctionTraits
    {
        // fallback if not specialized below
    };

    // Detect if T has operator(), then delegate to that
    template<typename T>
    struct FunctionTraits<T, std::void_t<decltype(&std::decay_t<T>::operator())>>
        : FunctionTraits<decltype(&std::decay_t<T>::operator())>
    {
    };

    // Handle free function: R(Args...)
    template<typename R, typename... Args>
    struct FunctionTraits<R(Args...), void> : FunctionTraitsImpl<R, Args...>
    {
    };

    // Handle function pointer: R(*)(Args...)
    template<typename R, typename... Args>
    struct FunctionTraits<R(*)(Args...), void> : FunctionTraitsImpl<R, Args...>
    {
    };

    // Handle non-const member function: R(C::*)(Args...)
    template<typename C, typename R, typename... Args>
    struct FunctionTraits<R(C::*)(Args...), void> : FunctionTraitsImpl<R, Args...>
    {
    };

    // Handle const member function: R(C::*)(Args...) const
    template<typename C, typename R, typename... Args>
    struct FunctionTraits<R(C::*)(Args...) const, void> : FunctionTraitsImpl<R, Args...>
    {
    };

    template<typename T>
    struct ParamTrait
    {
        static constexpr bool is_application = false;
        static constexpr bool is_coordinator = false;
        static constexpr bool is_query = false;
        static constexpr bool is_resource = true; // fallback
    };

    // Application
    template<>
    struct ParamTrait<spark::Application>
    {
        static constexpr bool is_application = true;
        static constexpr bool is_coordinator = false;
        static constexpr bool is_query = false;
        static constexpr bool is_resource = false;
    };
    // If user writes "Application&"
    template<>
    struct ParamTrait<Application&> : ParamTrait<Application> {};

    // Coordinator
    template<>
    struct ParamTrait<Coordinator>
    {
        static constexpr bool is_application = false;
        static constexpr bool is_coordinator = true;
        static constexpr bool is_query = false;
        static constexpr bool is_resource = false;
    };
    // If user writes "Coordinator&"
    template<>
    struct ParamTrait<Coordinator&> : ParamTrait<Coordinator> {};

    template<typename... Ts>
    struct ParamTrait<Query<Ts...>>
    {
        static constexpr bool is_application = false;
        static constexpr bool is_coordinator = false;
        static constexpr bool is_query = true;
        static constexpr bool is_resource = false;
    };

    template<typename T>
    struct QueryComponents;

    template<typename... Ts>
    struct QueryComponents<spark::Query<Ts...>>
    {
        static spark::Query<Ts...> Get(spark::Coordinator& coord)
        {
            return coord.template CreateQuery<Ts...>();
        }
    };

    template<typename... Ts>
    struct QueryComponents<spark::Query<Ts...>&>
    {
        static spark::Query<Ts...> Get(spark::Coordinator& coord)
        {
            return coord.template CreateQuery<Ts...>();
        }
    };

    template<typename... Ts>
    struct ParamTrait<Query<Ts...>&> : ParamTrait<Query<Ts...>> {};


    namespace detail
    {
        template<typename Arg>
        Arg& ComputeArgument(spark::Application& app);

        template<typename Func, size_t... I>
        void RunInjectedSystemImpl(spark::Application& app,
            Func&& func,
            std::index_sequence<I...>);
    }

    template<typename Func>
    void RunInjectedSystem(spark::Application& app, Func&& func);

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
            for (auto& system_fn : m_systems[LifecyclePhase::ON_START])
            {
                RunInjectedSystem(*this, system_fn);
            }
            m_layer_stack.Start();
            return *this;
        }

        Application& Run()
        {
            while (IsRunning())
            {
                // BEFORE_UPDATE
                for (auto& system_fn : m_systems[LifecyclePhase::BEFORE_UPDATE])
                {
                    RunInjectedSystem(*this, system_fn);
                }

                m_layer_stack.Update(m_dt);

                // UPDATE
                for (auto& system_fn : m_systems[LifecyclePhase::UPDATE])
                {
                    RunInjectedSystem(*this, system_fn);
                }

                // AFTER_UPDATE
                for (auto& system_fn : m_systems[LifecyclePhase::AFTER_UPDATE])
                {
                    RunInjectedSystem(*this, system_fn);
                }
            }
            return *this;
        }

        Application& Close()
        {
            // ON_SHUTDOWN
            for (auto& system_fn : m_systems[LifecyclePhase::ON_SHUTDOWN])
            {
                RunInjectedSystem(*this, system_fn);
            }
            m_layer_stack.Stop();
            return *this;
        }

        // Register any system (lambda, function, etc.) for a given phase
        template<typename Func>
        Application& RegisterSystem(Func&& system_func,
            LifecyclePhase phase = LifecyclePhase::UPDATE)
        {
            using F = std::decay_t<Func>;

            // We'll store a std::function that takes no args, 
            // but captures system_func. Then we call RunInjectedSystem(*this, system_func).
            auto wrapper = [fn = std::forward<F>(system_func)]() mutable
                {
                    // We'll do injection by calling the free function:
                    RunInjectedSystem(*(Application*)s_app_ptr, fn);
                };

            m_systems[phase].push_back(std::move(wrapper));
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

        spark::Coordinator& GetCoordinator()
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
        float m_dt = 0.0f;

        // Unchanged chunk-based ECS
        Coordinator m_coordinator;

        // Resource storage
        std::unordered_map<std::type_index, std::any> m_resources;

        // Systems by phase: each system is stored as function<void()>
        // We'll do injection by capturing the system and calling RunInjectedSystem(*this, system).
        std::unordered_map<
            LifecyclePhase,
            std::vector<std::function<void()>>>
            m_systems;

        static inline Application* s_app_ptr = nullptr;

    public:
        // We can set s_app_ptr in the constructor:
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
        Arg& ComputeArgument(spark::Application& app)
        {
            using Trait = ParamTrait<Arg>;

            if constexpr (Trait::is_application)
            {
                // Return the app object itself
                return app;
            }
            else if constexpr (Trait::is_coordinator)
            {
                // Return the chunk-based ECS coordinator
                return app.GetCoordinator();
            }
            else if constexpr (Trait::is_query)
            {
                // Build a Query from the coordinator and return a reference
                static auto query =
                    QueryComponents<Arg>::Get(app.GetCoordinator());
                return query;
            }
            else
            {
                // It's a resource => do app.GetResource<Arg>()
                return app.GetResource<Arg>();
            }
        }

        template<typename Func, size_t... I>
        void RunInjectedSystemImpl(spark::Application& app,
            Func&& func,
            std::index_sequence<I...>)
        {
            using Decayed = std::decay_t<Func>;
            using Traits = FunctionTraits<Decayed>;

            auto args_tuple = std::forward_as_tuple(
                ComputeArgument<typename Traits::template ArgType<I>>(app)...
            );

            std::apply(std::forward<Func>(func), args_tuple);
        }
    } // namespace detail



    template<typename Func>
    void RunInjectedSystem(spark::Application& app, Func&& func)
    {
        using Decayed = std::decay_t<Func>;
        using Traits = FunctionTraits<Decayed>;

        detail::RunInjectedSystemImpl(
            app,
            std::forward<Func>(func),
            std::make_index_sequence<Traits::arg_count>{}
        );
    }
} // namespace spark

#endif // SPARK_APPLICATION_HPP
