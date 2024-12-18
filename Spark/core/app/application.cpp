#include "application.hpp"

namespace Spark {

	Application::Application(GraphicsAPI gapi, f32 delta_time, UniquePtr<ThreadPool> tp)
		: m_thread_pool(Move(tp))
		, m_event_handler(MakeUnique<EventHandler>(*m_thread_pool))
		, m_ecs(MakeUnique<Ecs>(m_event_handler))
		, m_resource_manager(MakeUnique<Manager<Resource>>())
		, m_delta_time(delta_time)
		, m_gapi(gapi) {}

	Application::~Application() {
		RunShutdownFunctions();
		m_ecs->Shutdown();
	}

	void Application::Start() {
		if (!m_window) {
			LOG_FATAL("Window has not been created!");
		}

		if (!m_renderer) {
			LOG_FATAL("Renderer has not been created!");
		}

		m_event_handler->SubscribeToEvent<ComponentEvent<TransformComponent>>(
			[this](const EventPtr<ComponentEvent<TransformComponent>>& event) {
				std::lock_guard lock(m_resource_manager->m_mutex);
				m_resource_manager->GetManager<Scene>().OnEvent(event);
			},
			{true, false});

		m_event_handler->SubscribeToEvent<WindowResizedEvent>([this](const EventPtr<WindowResizedEvent>& event) { m_resource_manager->GetManager<Camera>().OnEvent(event); });
		
		RunStartupFunctions();
		m_ecs->Start();

		// Clear these so it uses less memory
		m_startup_functions.Clear();

		while (Running()) {
			Update();

		}
	}

	void Application::Update() {
		RunUpdateFunctions();
		m_ecs->Update(m_delta_time);
		m_window->Update();
	}

	Application& Application::AddStartupFunction(const ApplicationFunction& fn, const FunctionSettings settings) {
		m_startup_functions.PushBack({fn, settings});
		return *this;
	}

	Application& Application::AddUpdateFunction(const ApplicationFunction& fn, const FunctionSettings settings) {
		m_update_functions.PushBack({fn, settings});
		return *this;
	}

	Application& Application::AddShutdownFunction(const ApplicationFunction& fn, const FunctionSettings settings) {
		m_shutdown_functions.PushBack({fn, settings});
		return *this;
	}

	Application& Application::AddAllEventsFunction(const ApplicationEventFunction<IEvent>& fn, const FunctionSettings settings) {
		m_event_handler->SubscribeToAllEvents(
			[this, fn, settings](const EventPtr<IEvent>& event) {
				std::unique_lock<std::mutex> lock;
				if (settings.threaded)
					lock = std::unique_lock(m_mutex);
				fn(*this, event);
			},
			settings);
		return *this;
	}

	void Application::RunStartupFunctions() {
		for (const auto& [fn, settings] : m_startup_functions) {
			if (settings.threaded) {
				m_thread_pool->Enqueue(TaskPriority::HIGH, settings.wait, [this, &fn, settings]() {
					std::unique_lock<std::mutex> lock;
					if (settings.threaded)
						lock = std::unique_lock(m_mutex);
					fn(*this);
				});
			} else {
				fn(*this);
			}
		}

		m_thread_pool->SyncRegisteredTasks();
	}

	void Application::RunUpdateFunctions() {
		for (const auto& [fn, settings] : m_update_functions) {
			if (settings.threaded) {
				m_thread_pool->Enqueue(TaskPriority::NORMAL, settings.wait, [this, &fn, settings]() {
					std::unique_lock<std::mutex> lock;
					if (settings.threaded)
						lock = std::unique_lock(m_mutex);
					fn(*this);
				});
			} else {
				fn(*this);
			}
		}

		for (const auto& [fn, settings] : m_query_functions) {
			if (settings.threaded) {
				m_thread_pool->Enqueue(TaskPriority::NORMAL, settings.wait, [this, &fn, settings]() {
					std::unique_lock<std::mutex> lock;
					if (settings.threaded)
						lock = std::unique_lock(m_mutex);
					fn->Execute(*this);
				});
			} else {
				fn->Execute(*this);
			}
		}

		m_thread_pool->SyncRegisteredTasks();
	}

	void Application::RunShutdownFunctions() {
		for (const auto& [fn, settings] : m_shutdown_functions) {
			if (settings.threaded) {
				m_thread_pool->Enqueue(TaskPriority::NORMAL, settings.wait, [this, &fn, settings]() {
					std::unique_lock<std::mutex> lock;
					if (settings.threaded)
						lock = std::unique_lock(m_mutex);
					fn(*this);
				});
			} else {
				fn(*this);
			}
		}

		m_thread_pool->SyncRegisteredTasks();
	}

	Ecs& Application::GetEcs() const { return *m_ecs; }

	Window& Application::GetWindow() const { return *m_window; }

	Renderer& Application::GetRenderer() const { return *m_renderer; }

	EventHandler& Application::GetEventHandler() const { return *m_event_handler; }

	const bool Application::Running() const { return m_window->Running(); }

	ThreadPool& Application::GetThreadPool() const { return *m_thread_pool; }

	void Application::SetDeltaTime(f32 delta_time) { m_delta_time = delta_time; }

} // namespace Spark