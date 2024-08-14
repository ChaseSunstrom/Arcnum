#ifndef SPARK_APPLICATION_HPP
#define SPARK_APPLICATION_HPP

#include <core/pch.hpp>
#include <core/ecs/ecs.hpp>
#include <core/window/window.hpp>
#include <core/ecs/query.hpp>
#include <core/ecs/component.hpp>

namespace Spark
{
    class Application
    {
    public:
        using ApplicationFunction = std::function<void(Application&)>;

        template <IsComponent T>
        using ApplicationUpdateFunction = std::function<void(Application&, Query<T>&)>;

        Application(std::unique_ptr<IWindow> window) : m_ecs(std::make_unique<Ecs>()), m_window(std::move(window)) {}
        ~Application();

        Application& AddStartupFunction(ApplicationFunction fn);
        Application& AddShutdownFunction(ApplicationFunction fn);

        template <IsComponent T>
        Application& AddUpdateFunction(ApplicationUpdateFunction<T> fn);

        void Start();
        Ecs& GetEcs();
    private:
        void RunStartupFunctions();
        void RunUpdateFunctions();
        void RunShutdownFunctions();
        void Update();
    private:
        std::vector<ApplicationFunction> m_startup_functions;
        std::vector<ApplicationFunction> m_shutdown_functions;

        struct IUpdateFunctionWrapper
        {
            virtual ~IUpdateFunctionWrapper() = default;
            virtual void Execute(Application& app) = 0;
        };

        template <IsComponent T>
        struct UpdateFunctionWrapper : IUpdateFunctionWrapper
        {
            ApplicationUpdateFunction<T> fn;
            Ecs& ecs;

            UpdateFunctionWrapper(Ecs& ecs, ApplicationUpdateFunction<T> fn) : ecs(ecs), fn(fn) {}

            void Execute(Application& app) override
            {
                fn(app, ecs.GetComponents<T>());
            }
        };

        std::vector<std::unique_ptr<IUpdateFunctionWrapper>> m_update_functions;
        std::unique_ptr<Ecs> m_ecs;
        std::unique_ptr<IWindow> m_window;
    };

    template <IsComponent T>
    Application& Application::AddUpdateFunction(ApplicationUpdateFunction<T> fn)
    {
        m_update_functions.push_back(std::make_unique<UpdateFunctionWrapper<T>>(*m_ecs, fn));
        return *this;
    }
}

#endif