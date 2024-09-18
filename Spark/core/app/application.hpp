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
		using ApplicationFunction                                                           = Callable<void(Application&)>;
		template<typename _Ty> using ApplicationEventFunction                                 = Callable<void(Application&, const EventPtr<_Ty>&)>;
		template<typename... EventTypes> using ApplicationMultiEventFunction                = Callable<void(Application&, const MultiEventPtr<EventTypes...>&)>;
		using ApplicationFunctionList                                                       = Vector<Pair<ApplicationFunction, FunctionSettings>>;
		template<IsComponent _Ty> using ApplicationQueryFunction                              = Callable<void(Application&, Query<_Ty>&)>;
		template<IsComponent _Ty, typename... EventTypes> using ApplicationQueryEventFunction = Callable<void(Application&, Query<_Ty>&, const MultiEventPtr<EventTypes...>&)>;

		Application(GraphicsAPI gapi, f32 delta_time = 60, UniquePtr<ThreadPool> tp = MakeUnique<ThreadPool>(std::thread::hardware_concurrency()));
		~Application();

		void         Start();
		const bool   Running() const;
		Application& AddStartupFunction(const ApplicationFunction& fn, const FunctionSettings settings = {});
		Application& AddUpdateFunction(const ApplicationFunction& fn, const FunctionSettings settings = {});
		Application& AddShutdownFunction(const ApplicationFunction& fn, const FunctionSettings settings = {});

		template<IsSystem _Ty, typename... Args> Application& AddSystem(Args&&... args);

		template<IsSystem _Ty> Application& AddSystem(_Ty* t);

		template<IsSystem _Ty> Application& AddSystem(UniquePtr<_Ty> t);

		template<IsEvent EventType> Application& AddEventFunction(const ApplicationEventFunction<EventType>& fn, const FunctionSettings settings = {});

		template<IsEvent... EventTypes> Application& AddEventFunction(const ApplicationMultiEventFunction<EventTypes...>& fn, const FunctionSettings settings = {});

		Application& AddAllEventsFunction(const ApplicationEventFunction<IEvent>& fn, const FunctionSettings settings = {});

		template<IsComponent _Ty> Application& AddQueryFunction(const ApplicationQueryFunction<_Ty>& fn, const FunctionSettings settings = {});

		// Single event query function
		template<IsComponent _Ty, IsEvent EventType> Application& AddQueryEventFunction(const Callable<void(Application&, Query<_Ty>&, const EventPtr<EventType>&)>& fn, const FunctionSettings settings = {});

		// Multiple events query function
		template<IsComponent _Ty, IsEvent... EventTypes>
		Application& AddQueryEventFunction(const Callable<void(Application&, Query<_Ty>&, const MultiEventPtr<EventTypes...>&)>& fn, const FunctionSettings settings = {});

		template<IsWindow _Ty> Application&   CreateWindow(const String& title, i32 width, i32 height);
		template<IsRenderer _Ty> Application& CreateRenderer();
		template<typename _Ty> Manager<_Ty>&    GetManager() const;
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

		template<IsComponent _Ty> struct UpdateFunctionWrapper : IUpdateFunctionWrapper {
			ApplicationQueryFunction<_Ty> fn;
			Ecs&                        ecs;

			UpdateFunctionWrapper(Ecs& ecs, ApplicationQueryFunction<_Ty> fn)
				: ecs(ecs)
				, fn(fn) {}

			void Execute(Application& app) override { fn(app, ecs.GetComponents<_Ty>()); }
		};

		template<IsComponent _Ty, typename... EventTypes> struct QueryEventFunctionWrapper : IQueryEventFunctionWrapper {
			Callable<void(Application&, Query<_Ty>&, const std::conditional_t<sizeof...(EventTypes) == 1, EventPtr<typename std::tuple_element<0, std::tuple<EventTypes...>>::type>, MultiEventPtr<EventTypes...>>&)> fn;
			Ecs& ecs;

			QueryEventFunctionWrapper(Ecs& ecs, const decltype(fn)& fn)
				: ecs(ecs)
				, fn(fn) {}

			void Execute(Application& app, const EventPtr<IEvent>& event) override {
				if constexpr (sizeof...(EventTypes) == 1) {
					if (auto typed_event = std::dynamic_pointer_cast<typename std::tuple_element<0, std::tuple<EventTypes...>>::type>(event)) {
						fn(app, ecs.GetComponents<_Ty>(), typed_event);
					}
				} else {
					if (auto multi_event = std::dynamic_pointer_cast<MultiEvent<EventTypes...>>(event)) {
						fn(app, ecs.GetComponents<_Ty>(), multi_event);
					}
				}
			}
		};

		UniquePtr<ThreadPool>                                                       m_thread_pool;
		UniquePtr<EventHandler>                                                           m_event_handler;
		UniquePtr<Ecs>                                                                    m_ecs;
		UniquePtr<Window>                                                                 m_window;
		UniquePtr<Renderer>                                                               m_renderer;
		UniquePtr<Manager<Resource>>                                                      m_resource_manager;
		ApplicationFunctionList                                                           m_startup_functions;
		ApplicationFunctionList                                                           m_update_functions;
		ApplicationFunctionList                                                           m_shutdown_functions;
		Vector<Pair<UniquePtr<IUpdateFunctionWrapper>, FunctionSettings>> m_query_functions;
		Vector<UniquePtr<IQueryEventFunctionWrapper>>                                m_query_event_functions;
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

	template<IsComponent _Ty, IsEvent EventType> Application& Application::AddQueryEventFunction(const Callable<void(Application&, Query<_Ty>&, const EventPtr<EventType>&)>& fn, const FunctionSettings settings) {
		auto  query_event_wrapper     = MakeUnique<QueryEventFunctionWrapper<_Ty, EventType>>(*m_ecs, fn);
		auto& query_event_wrapper_ref = *query_event_wrapper;
		m_query_event_functions.PushBack(Move(query_event_wrapper));

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

	template<IsComponent _Ty, IsEvent... EventTypes>
	Application& Application::AddQueryEventFunction(const Callable<void(Application&, Query<_Ty>&, const MultiEventPtr<EventTypes...>&)>& fn, const FunctionSettings settings) {
		auto  query_event_wrapper     = MakeUnique<QueryEventFunctionWrapper<_Ty, EventTypes...>>(*m_ecs, fn);
		auto& query_event_wrapper_ref = *query_event_wrapper;
		m_query_event_functions.PushBack(Move(query_event_wrapper));

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

	template<IsSystem _Ty, typename... Args> Application& Application::AddSystem(Args&&... args) {
		m_ecs->AddSystem(MakeUnique<_Ty>(*m_event_handler, std::forward<Args>(args)...));
		return *this;
	}

	template<IsSystem _Ty> Application& Application::AddSystem(_Ty* t) {
		m_ecs->AddSystem(UniquePtr<_Ty>(t));
		return *this;
	}

	template<IsSystem _Ty> Application& Application::AddSystem(UniquePtr<_Ty> t) {
		m_ecs->AddSystem(Move(t));
		return *this;
	}

	template<IsWindow _Ty> Application& Application::CreateWindow(const String& title, i32 width, i32 height) {
		m_window = MakeUnique<_Ty>(title, width, height, *m_event_handler);
		return *this;
	}

	template<IsRenderer _Ty> Application& Application::CreateRenderer() {
		m_renderer = MakeUnique<_Ty>(m_gapi, m_window->GetFrameBuffer(), *m_resource_manager);
		return *this;
	}

	template<IsComponent _Ty> Application& Application::AddQueryFunction(const ApplicationQueryFunction<_Ty>& fn, const FunctionSettings settings) {
		m_query_functions.PushBack({MakeUnique<UpdateFunctionWrapper<_Ty>>(*m_ecs, fn), settings});
		return *this;
	}

	template<typename _Ty> Manager<_Ty>& Application::GetManager() const { return m_resource_manager->GetManager<_Ty>(); }

} // namespace Spark

#endif