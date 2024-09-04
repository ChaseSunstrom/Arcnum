#include "application.hpp"
#include "core/ecs/components/transform_component.hpp"

namespace Spark {

Application::Application(GraphicsAPI gapi, std::unique_ptr<ThreadPool> tp)
	: m_thread_pool(std::move(tp))
	, m_event_handler(std::make_unique<EventHandler>(*m_thread_pool))
	, m_ecs(std::make_unique<Ecs>(*m_event_handler))
	, m_resource_manager(std::make_unique<Manager<Resource>>())
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
		[this](const std::shared_ptr<ComponentEvent<TransformComponent>>& event) {
			std::unique_lock lock(m_resource_manager->m_mutex);
			m_resource_manager->GetManager<Scene>().OnEvent(event);
		},
		{true, false});

	RunStartupFunctions();
	m_ecs->Start();

	while (Running()) {
		Update();
	}
}

void Application::Update() {
	RunUpdateFunctions();
	m_ecs->Update();
	m_window->Update();
	m_renderer->Render();
}

Application& Application::AddStartupFunction(const ApplicationFunction& fn, const FunctionSettings settings) {
	m_startup_functions.push_back({fn, settings});
	return *this;
}

Application& Application::AddUpdateFunction(const ApplicationFunction& fn, const FunctionSettings settings) {
	m_update_functions.push_back({fn, settings});
	return *this;
}

Application& Application::AddShutdownFunction(const ApplicationFunction& fn, const FunctionSettings settings) {
	m_shutdown_functions.push_back({fn, settings});
	return *this;
}

Application& Application::AddAllEventsFunction(const ApplicationEventFunction& fn, const FunctionSettings settings) {
	m_event_handler->SubscribeToAllEvents(
		[this, fn, settings](const std::shared_ptr<BaseEvent>& event) {
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

const bool Application::Running() const {
	return m_window->Running();
}

ThreadPool& Application::GetThreadPool() const { return *m_thread_pool; }

} // namespace Spark