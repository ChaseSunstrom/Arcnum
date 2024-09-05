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
		using ApplicationFunction                                                           = std::function<void(Application&)>;
		template<typename T> using ApplicationEventFunction                                 = std::function<void(Application&, const EventPtr<T>&)>;
		template<typename... EventTypes> using ApplicationMultiEventFunction                = std::function<void(Application&, const MultiEventPtr<EventTypes...>&)>;
		using ApplicationFunctionList                                                       = std::vector<std::pair<ApplicationFunction, FunctionSettings>>;
		template<IsComponent T> using ApplicationQueryFunction                              = std::function<void(Application&, Query<T>&)>;
		template<IsComponent T, typename... EventTypes> using ApplicationQueryEventFunction = std::function<void(Application&, Query<T>&, const MultiEventPtr<EventTypes...>&)>;

		Application(GraphicsAPI gapi, f32 delta_time = 60, std::unique_ptr<ThreadPool> tp = std::make_unique<ThreadPool>(std::thread::hardware_concurrency()));
		~Application();

		void         Start();
		const bool   Running() const;
		Application& AddStartupFunction(const ApplicationFunction& fn, const FunctionSettings settings = {});
		Application& AddUpdateFunction(const ApplicationFunction& fn, const FunctionSettings settings = {});
		Application& AddShutdownFunction(const ApplicationFunction& fn, const FunctionSettings settings = {});

		template<IsSystem T, typename... Args> Application& AddSystem(Args&&... args);

		template<IsSystem T> Application& AddSystem(T* t);

		template<IsSystem T> Application& AddSystem(std::unique_ptr<T> t);

		template<IsEvent EventType> Application& AddEventFunction(const ApplicationEventFunction<EventType>& fn, const FunctionSettings settings = {});

		template<IsEvent... EventTypes> Application& AddEventFunction(const ApplicationMultiEventFunction<EventTypes...>& fn, const FunctionSettings settings = {});

		Application& AddAllEventsFunction(const ApplicationEventFunction<IEvent>& fn, const FunctionSettings settings = {});

		template<IsComponent T> Application& AddQueryFunction(const ApplicationQueryFunction<T>& fn, const FunctionSettings settings = {});

		// Single event query function
		template<IsComponent T, IsEvent EventType> Application& AddQueryEventFunction(const std::function<void(Application&, Query<T>&, const EventPtr<EventType>&)>& fn, const FunctionSettings settings = {});

		// Multiple events query function
		template<IsComponent T, IsEvent... EventTypes>
		Application& AddQueryEventFunction(const std::function<void(Application&, Query<T>&, const MultiEventPtr<EventTypes...>&)>& fn, const FunctionSettings settings = {});

		template<IsWindow T> Application&   CreateWindow(const std::string& title, i32 width, i32 height);
		template<IsRenderer T> Application& CreateRenderer();
		template<typename T> Manager<T>&    GetManager() const;
		Ecs&                                GetEcs() const;
		Window&                             GetWindow() const;
		Renderer&                           GetRenderer() const;
		EventHandler&                       GetEventHandler() const;
		ThreadPool&                         GetThreadPool() const;

		void SetDeltaTime(f32 delta_time);

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
			virtual ~IQueryEventFunctionWrapper()                                 = default;
			virtual void Execute(Application& app, const EventPtr<IEvent>& event) = 0;
		};

		template<IsComponent T> struct UpdateFunctionWrapper : IUpdateFunctionWrapper {
			ApplicationQueryFunction<T> fn;
			Ecs&                        ecs;

			UpdateFunctionWrapper(Ecs& ecs, ApplicationQueryFunction<T> fn)
				: ecs(ecs)
				, fn(fn) {}

			void Execute(Application& app) override { fn(app, ecs.GetComponents<T>()); }
		};

		template<IsComponent T, typename... EventTypes> struct QueryEventFunctionWrapper : IQueryEventFunctionWrapper {
			std::function<void(Application&, Query<T>&, const std::conditional_t<sizeof...(EventTypes) == 1, EventPtr<typename std::tuple_element<0, std::tuple<EventTypes...>>::type>, MultiEventPtr<EventTypes...>>&)> fn;
			Ecs& ecs;

			QueryEventFunctionWrapper(Ecs& ecs, const decltype(fn)& fn)
				: ecs(ecs)
				, fn(fn) {}

			void Execute(Application& app, const EventPtr<IEvent>& event) override {
				if constexpr (sizeof...(EventTypes) == 1) {
					if (auto typed_event = std::dynamic_pointer_cast<typename std::tuple_element<0, std::tuple<EventTypes...>>::type>(event)) {
						fn(app, ecs.GetComponents<T>(), typed_event);
					}
				} else {
					if (auto multi_event = std::dynamic_pointer_cast<MultiEvent<EventTypes...>>(event)) {
						fn(app, ecs.GetComponents<T>(), multi_event);
					}
				}
			}
		};

		std::unique_ptr<ThreadPool>                                                       m_thread_pool;
		std::unique_ptr<EventHandler>                                                     m_event_handler;
		std::unique_ptr<Ecs>                                                              m_ecs;
		std::unique_ptr<Window>                                                           m_window;
		std::unique_ptr<Renderer>                                                         m_renderer;
		std::unique_ptr<Manager<Resource>>                                                m_resource_manager;
		ApplicationFunctionList                                                           m_startup_functions;
		ApplicationFunctionList                                                           m_update_functions;
		ApplicationFunctionList                                                           m_shutdown_functions;
		std::vector<std::pair<std::unique_ptr<IUpdateFunctionWrapper>, FunctionSettings>> m_query_functions;
		std::vector<std::unique_ptr<IQueryEventFunctionWrapper>>                          m_query_event_functions;
		GraphicsAPI                                                                       m_gapi;
		f32                                                                               m_delta_time;
		std::mutex                                                                        m_mutex;
	};

	// Implementation of template methods

	template<IsEvent EventType> Application& Application::AddEventFunction(const ApplicationEventFunction<EventType>& fn, const FunctionSettings settings) {
		m_event_handler->SubscribeToEvent<EventType>(
			[this, fn, settings](const EventPtr<EventType>& event) {
				std::unique_lock<std::mutex> lock;
				if (settings.threaded)
					lock = std::unique_lock(m_mutex);
				fn(*this, event);
			},
			settings);
		return *this;
	}

	template<IsEvent... EventTypes> Application& Application::AddEventFunction(const ApplicationMultiEventFunction<EventTypes...>& fn, const FunctionSettings settings) {
		m_event_handler->SubscribeToMultipleEvents<EventTypes...>(
			[this, fn, settings](const MultiEventPtr<EventTypes...>& event) {
				std::unique_lock<std::mutex> lock;
				if (settings.threaded)
					lock = std::unique_lock(m_mutex);
				fn(*this, event);
			},
			settings);
		return *this;
	}

	template<IsComponent T, IsEvent EventType> Application& Application::AddQueryEventFunction(const std::function<void(Application&, Query<T>&, const EventPtr<EventType>&)>& fn, const FunctionSettings settings) {
		auto  query_event_wrapper     = std::make_unique<QueryEventFunctionWrapper<T, EventType>>(*m_ecs, fn);
		auto& query_event_wrapper_ref = *query_event_wrapper;
		m_query_event_functions.push_back(std::move(query_event_wrapper));

		m_event_handler->SubscribeToEvent<EventType>(
			[this, &query_event_wrapper_ref, settings](const EventPtr<EventType>& event) {
				std::unique_lock<std::mutex> lock;
				if (settings.threaded)
					lock = std::unique_lock(m_mutex);
				query_event_wrapper_ref.Execute(*this, event);
			},
			settings);

		return *this;
	}

	template<IsComponent T, IsEvent... EventTypes>
	Application& Application::AddQueryEventFunction(const std::function<void(Application&, Query<T>&, const MultiEventPtr<EventTypes...>&)>& fn, const FunctionSettings settings) {
		auto  query_event_wrapper     = std::make_unique<QueryEventFunctionWrapper<T, EventTypes...>>(*m_ecs, fn);
		auto& query_event_wrapper_ref = *query_event_wrapper;
		m_query_event_functions.push_back(std::move(query_event_wrapper));

		m_event_handler->SubscribeToMultipleEvents<EventTypes...>(
			[this, &query_event_wrapper_ref, settings](const MultiEventPtr<EventTypes...>& event) {
				std::unique_lock<std::mutex> lock;
				if (settings.threaded)
					lock = std::unique_lock(m_mutex);
				query_event_wrapper_ref.Execute(*this, event);
			},
			settings);

		return *this;
	}

	template<IsSystem T, typename... Args> Application& Application::AddSystem(Args&&... args) {
		m_ecs->AddSystem(std::make_unique<T>(*m_event_handler, std::forward<Args>(args)...));
		return *this;
	}

	template<IsSystem T> Application& Application::AddSystem(T* t) {
		m_ecs->AddSystem(std::unique_ptr<T>(t));
		return *this;
	}

	template<IsSystem T> Application& Application::AddSystem(std::unique_ptr<T> t) {
		m_ecs->AddSystem(std::move(t));
		return *this;
	}

	template<IsWindow T> Application& Application::CreateWindow(const std::string& title, i32 width, i32 height) {
		m_window = std::make_unique<T>(title, width, height, *m_event_handler);
		return *this;
	}

	template<IsRenderer T> Application& Application::CreateRenderer() {
		m_renderer = std::make_unique<T>(m_gapi, m_window->GetFrameBuffer(), *m_resource_manager);
		return *this;
	}

	template<IsComponent T> Application& Application::AddQueryFunction(const ApplicationQueryFunction<T>& fn, const FunctionSettings settings) {
		m_query_functions.push_back({std::make_unique<UpdateFunctionWrapper<T>>(*m_ecs, fn), settings});
		return *this;
	}

	template<typename T> Manager<T>& Application::GetManager() const { return m_resource_manager->GetManager<T>(); }

} // namespace Spark

#endif