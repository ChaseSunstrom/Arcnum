#ifndef SPARK_APPLICATION_HPP
#define SPARK_APPLICATION_HPP

#include <core/pch.hpp>
#include <core/ecs/ecs.hpp>
#include <core/window/window.hpp>
#include <core/ecs/query.hpp>
#include <core/system/manager.hpp>
#include <core/api.hpp>
#include <core/render/renderer.hpp>
#include <core/util/thread_pool.hpp>
#include <core/ecs/component.hpp>
#include <core/resource/resource.hpp>
#include <core/event/event_handler.hpp>

namespace Spark
{
	// Scope for the Application mutex
	

	class Application
	{
	public:
		using ApplicationFunction = std::function<void(Application&)>; // The bool indicates whether to run on a separate thread
		using ApplicationEventFunction = std::function<void(Application&, const std::shared_ptr<Event>)>;

		template <IsComponent T>
		using ApplicationQueryFunction = std::function<void(Application&, Query<T>&)>;

		template <IsComponent T>
		using ApplicationQueryEventFunction = std::function<void(Application&, Query<T>&, const std::shared_ptr<Event>)>;


		Application(GraphicsAPI gapi, std::unique_ptr<ThreadPool> tp = std::make_unique<ThreadPool>(std::thread::hardware_concurrency())) :
			m_thread_pool(std::move(tp)),
			m_event_handler(std::make_unique<EventHandler>(*m_thread_pool)),
			m_ecs(std::make_unique<Ecs>(*m_event_handler)),
			m_gapi(gapi) {}

		~Application();

		Application& AddStartupFunction(const ApplicationFunction& fn, const FunctionSettings settings = {});
		Application& AddUpdateFunction(const ApplicationFunction& fn, const FunctionSettings settings = {});
		Application& AddShutdownFunction(const ApplicationFunction& fn, const FunctionSettings settings = {});
		Application& AddEventFunction(i64 event_type, const ApplicationEventFunction& fn, const FunctionSettings settings = {});

		template <IsComponent T>
		Application& AddQueryFunction(const ApplicationQueryFunction<T>& fn, const FunctionSettings settings = {});

		template <IsComponent T>
		Application& AddQueryEventFunction(i64 event_type, const ApplicationQueryEventFunction<T>& fn, const FunctionSettings settings = {});

		template <IsWindow T>
		Application& CreateWindow(const std::string& title, i32 width, i32 height);

		template <IsRenderer T>
		Application& CreateRenderer();

		void Start();

		Ecs& GetEcs() const;
		Window& GetWindow() const;
		Renderer& GetRenderer() const;
		EventHandler& GetEventHandler() const;
		ThreadPool& GetThreadPool() const;

		template <typename T>
		Manager<T>& GetManager() const;

		const bool Running() const;
	private:
		void RunStartupFunctions();
		void RunUpdateFunctions();
		void RunShutdownFunctions();
		void Update();
	private:
		std::vector<std::pair<ApplicationFunction, FunctionSettings>> m_startup_functions;
		std::vector<std::pair<ApplicationFunction, FunctionSettings>> m_update_functions;
		std::vector<std::pair<ApplicationFunction, FunctionSettings>> m_shutdown_functions;
		std::vector<ApplicationEventFunction> m_event_functions;

		struct IUpdateFunctionWrapper
		{
			virtual ~IUpdateFunctionWrapper() = default;
			virtual void Execute(Application& app) = 0;
		};

		struct IQueryEventFunctionWrapper
		{
			virtual ~IQueryEventFunctionWrapper() = default;
			virtual void Execute(Application& app, const std::shared_ptr<Event> event) = 0;
		};

		template <IsComponent T>
		struct UpdateFunctionWrapper : IUpdateFunctionWrapper
		{
			ApplicationQueryFunction<T> fn;
			Ecs& ecs;

			UpdateFunctionWrapper(Ecs& ecs, ApplicationQueryFunction<T> fn) : ecs(ecs), fn(fn) {}

			void Execute(Application& app) override
			{
				fn(app, ecs.GetComponents<T>());
			}
		};

		template <IsComponent T>
		struct QueryEventFunctionWrapper : IQueryEventFunctionWrapper
		{
			ApplicationQueryEventFunction<T> fn;
			Ecs& ecs;
			i64 event_type;

			QueryEventFunctionWrapper(Ecs& ecs, const ApplicationQueryEventFunction<T>& fn, i64 event_type) : ecs(ecs), fn(fn), event_type(event_type) {}

			void Execute(Application& app, const std::shared_ptr<Event> event) override
			{
				fn(app, ecs.GetComponents<T>(), event);
			}
		};

		std::unique_ptr<ThreadPool> m_thread_pool;
		std::unique_ptr<EventHandler> m_event_handler;
		std::unique_ptr<Ecs> m_ecs;
		std::unique_ptr<Window> m_window;
		std::unique_ptr<Renderer> m_renderer;
		std::unique_ptr<Manager<Resource>> m_resource_manager;
		std::vector<std::pair<std::unique_ptr<IUpdateFunctionWrapper>, FunctionSettings>> m_query_functions;
		std::vector<std::unique_ptr<IQueryEventFunctionWrapper>> m_query_event_functions;
		GraphicsAPI m_gapi;
		std::mutex m_mutex;
	};

	template <IsWindow T>
	Application& Application::CreateWindow(const std::string& title, i32 width, i32 height)
	{
		m_window = std::make_unique<T>(title, width, height, *m_event_handler);
		return *this;
	}

	template <IsRenderer T>
	Application& Application::CreateRenderer()
	{
		m_renderer = std::make_unique<T>(m_gapi);
		return *this;
	}

	template <IsComponent T>
	Application& Application::AddQueryFunction(const ApplicationQueryFunction<T>& fn, const FunctionSettings settings)
	{
		m_query_functions.push_back({ std::make_unique<UpdateFunctionWrapper<T>>(*m_ecs, fn), settings });
		return *this;
	}

	template <IsComponent T>
	Application& Application::AddQueryEventFunction(i64 event_type, const ApplicationQueryEventFunction<T>& fn, const FunctionSettings settings)
	{
		auto query_event_wrapper = std::make_unique<QueryEventFunctionWrapper<T>>(*m_ecs, fn, event_type);
		auto& query_event_wrapper_ref = *query_event_wrapper;
		m_query_event_functions.push_back(std::move(query_event_wrapper));

		m_event_handler->SubscribeToEvent(event_type, [this, fn, &query_event_wrapper_ref, settings](const std::shared_ptr<Event> event) {
			std::unique_lock<std::mutex> lock;
			if (settings.threaded)
				lock = std::unique_lock<std::mutex>(m_mutex);
			query_event_wrapper_ref.Execute(*this, event);
			}, settings);

		return *this;
	}

	template <typename T>
	Manager<T>& Application::GetManager() const
	{
		return *static_cast<Manager<T>*>(m_resource_manager.get());
	}
}

#endif