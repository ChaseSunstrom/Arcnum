// application.hpp
#ifndef SPARK_APPLICATION_HPP
#define SPARK_APPLICATION_HPP

#include <core/api.hpp>
#include <core/ecs/component.hpp>
#include <core/ecs/ecs.hpp>
#include <core/ecs/query.hpp>
#include <core/event/event_handler.hpp>
#include <core/pch.hpp>
#include <core/render/renderer.hpp>
#include <core/resource/resource.hpp>
#include <core/system/manager.hpp>
#include <core/util/thread_pool.hpp>
#include <core/window/window.hpp>

namespace Spark {
class Application {
  public:
	using ApplicationFunction      = std::function<void(Application&)>;
	using ApplicationEventFunction = std::function<void(Application&, const std::shared_ptr<BaseEvent>)>;
	using ApplicationFunctionList  = std::vector<std::pair<ApplicationFunction, FunctionSettings>>;
	template <IsComponent T>
	using ApplicationQueryFunction = std::function<void(Application&, Query<T>&)>;
	template <IsComponent T>
	using ApplicationQueryEventFunction = std::function<void(Application&, Query<T>&, const std::shared_ptr<BaseEvent>&)>;

	Application(GraphicsAPI gapi, std::unique_ptr<ThreadPool> tp = std::make_unique<ThreadPool>(std::thread::hardware_concurrency()));
	~Application();

	void Start();
	const bool Running() const;
	Application& AddStartupFunction(const ApplicationFunction& fn, const FunctionSettings settings = {});
	Application& AddUpdateFunction(const ApplicationFunction& fn, const FunctionSettings settings = {});
	Application& AddShutdownFunction(const ApplicationFunction& fn, const FunctionSettings settings = {});

	template <typename... EventTypes>
	Application& AddEventFunction(const std::function<void(Application&, const std::shared_ptr<BaseEvent>)>& fn, const FunctionSettings settings = {}) {
		(AddSingleEventFunction<EventTypes>(fn, settings), ...);
		return *this;
	}

	template <typename EventType>
	Application& AddSingleEventFunction(const std::function<void(Application&, const std::shared_ptr<EventType>)>& fn, const FunctionSettings settings = {});

	Application& AddAllEventsFunction(const ApplicationEventFunction& fn, const FunctionSettings settings = {});

	template <IsComponent T>
	Application& AddQueryFunction(const ApplicationQueryFunction<T>& fn, const FunctionSettings settings = {});

	template <IsComponent T, typename... EventTypes>
	Application& AddQueryEventFunction(const std::function<void(Application&, Query<T>&, const std::shared_ptr<BaseEvent>&)>& fn, const FunctionSettings settings = {}) {
		(AddSingleQueryEventFunction<T, EventTypes>(fn, settings), ...);
		return *this;
	}

	template <IsComponent T, typename EventType>
	Application& AddSingleQueryEventFunction(const std::function<void(Application&, Query<T>&, const std::shared_ptr<BaseEvent>&)>& fn, const FunctionSettings settings = {});

	template <IsWindow T>
	Application& CreateWindow(const std::string& title, i32 width, i32 height);
	template <IsRenderer T>
	Application& CreateRenderer();
	template <typename T>
	Manager<T>& GetManager() const;
	Ecs& GetEcs() const;
	Window& GetWindow() const;
	Renderer& GetRenderer() const;
	EventHandler& GetEventHandler() const;
	ThreadPool& GetThreadPool() const;

  private:
	void RunStartupFunctions();
	void RunUpdateFunctions();
	void RunShutdownFunctions();
	void Update();

	struct IUpdateFunctionWrapper {
		virtual ~IUpdateFunctionWrapper()      = default;
		virtual void Execute(Application& app) = 0;
	};

	struct IQueryEventFunctionWrapper {
		virtual ~IQueryEventFunctionWrapper()                                           = default;
		virtual void Execute(Application& app, const std::shared_ptr<BaseEvent>& event) = 0;
	};

	template <IsComponent T>
	struct UpdateFunctionWrapper : IUpdateFunctionWrapper {
		ApplicationQueryFunction<T> fn;
		Ecs& ecs;

		UpdateFunctionWrapper(Ecs& ecs, ApplicationQueryFunction<T> fn)
			: ecs(ecs)
			, fn(fn) {}

		void Execute(Application& app) override { fn(app, ecs.GetComponents<T>()); }
	};

	template <IsComponent T>
	struct QueryEventFunctionWrapper : IQueryEventFunctionWrapper {
		std::function<void(Application&, Query<T>&, const std::shared_ptr<BaseEvent>&)> fn;
		Ecs& ecs;

		QueryEventFunctionWrapper(Ecs& ecs, const std::function<void(Application&, Query<T>&, const std::shared_ptr<BaseEvent>&)>& fn)
			: ecs(ecs)
			, fn(fn) {}

		void Execute(Application& app, const std::shared_ptr<BaseEvent>& event) override {
			fn(app, ecs.GetComponents<T>(), event);
		}
	};

	std::unique_ptr<ThreadPool> m_thread_pool;
	std::unique_ptr<EventHandler> m_event_handler;
	std::unique_ptr<Ecs> m_ecs;
	std::unique_ptr<Window> m_window;
	std::unique_ptr<Renderer> m_renderer;
	std::unique_ptr<Manager<Resource>> m_resource_manager;
	ApplicationFunctionList m_startup_functions;
	ApplicationFunctionList m_update_functions;
	ApplicationFunctionList m_shutdown_functions;
	std::vector<std::pair<std::unique_ptr<IUpdateFunctionWrapper>, FunctionSettings>> m_query_functions;
	std::vector<std::unique_ptr<IQueryEventFunctionWrapper>> m_query_event_functions;
	GraphicsAPI m_gapi;
	std::mutex m_mutex;
};

// Implementation of template methods

template <IsWindow T>
Application& Application::CreateWindow(const std::string& title, i32 width, i32 height) {
	m_window = std::make_unique<T>(title, width, height, *m_event_handler);
	return *this;
}

template <IsRenderer T>
Application& Application::CreateRenderer() {
	m_renderer = std::make_unique<T>(m_gapi);
	return *this;
}

template <typename EventType>
Application& Application::AddSingleEventFunction(const std::function<void(Application&, const std::shared_ptr<EventType>)>& fn, const FunctionSettings settings) {
	m_event_handler->SubscribeToEvent<EventType>(
		[this, fn, settings](const std::shared_ptr<EventType> event) {
			std::unique_lock<std::mutex> lock;
			if (settings.threaded)
				lock = std::unique_lock(m_mutex);
			fn(*this, event);
		},
		settings);
	return *this;
}

template <IsComponent T>
Application& Application::AddQueryFunction(const ApplicationQueryFunction<T>& fn, const FunctionSettings settings) {
	m_query_functions.push_back({std::make_unique<UpdateFunctionWrapper<T>>(*m_ecs, fn), settings});
	return *this;
}

template <IsComponent T, typename EventType>
Application& Application::AddSingleQueryEventFunction(const std::function<void(Application&, Query<T>&, const std::shared_ptr<BaseEvent>&)>& fn, const FunctionSettings settings) {
	auto query_event_wrapper      = std::make_unique<QueryEventFunctionWrapper<T>>(*m_ecs, fn);
	auto& query_event_wrapper_ref = *query_event_wrapper;
	m_query_event_functions.push_back(std::move(query_event_wrapper));

	m_event_handler->SubscribeToEvent<EventType>(
		[this, &query_event_wrapper_ref, settings](const std::shared_ptr<BaseEvent>& event) {
			std::unique_lock<std::mutex> lock;
			if (settings.threaded)
				lock = std::unique_lock(m_mutex);
			query_event_wrapper_ref.Execute(*this, event);
		},
		settings);

	return *this;
}

template <typename T>
Manager<T>& Application::GetManager() const { return m_resource_manager->GetManager<T>(); }

} // namespace Spark

#endif