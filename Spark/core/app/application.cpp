#include "application.hpp"

namespace Spark
{
	Application::~Application()
	{
		RunShutdownFunctions();
	}

	void Application::Start()
	{
		if (!m_window)
		{
			LOG_ERROR("Window has not been created!");
			assert(false);
		}

		if (!m_renderer)
		{
			LOG_ERROR("Renderer has not been created!");
			assert(false);
		}

		RunStartupFunctions();

		while (m_window->Running())
		{
			Update();
		}
	}

	void Application::Update()
	{
		RunUpdateFunctions();
		m_window->Update();
		m_renderer->Render();
	}

	Application& Application::AddStartupFunction(const ApplicationFunction& fn, FunctionSettings threaded)
	{
		m_startup_functions.push_back({fn, threaded});
		return *this;
	}

	Application& Application::AddEventFunction(i64 event_type, const ApplicationEventFunction& fn)
	{
		m_event_functions.push_back(fn);
		m_event_handler->SubscribeToEvent(event_type, [this, fn](const std::shared_ptr<Event> event) {
				std::unique_lock<std::mutex> lock(m_mutex);
				fn(*this, event);
			});
		return *this;
	}

	Application& Application::AddUpdateFunction(const ApplicationFunction& fn, FunctionSettings threaded)
	{
		m_update_functions.push_back({fn, threaded });
		return *this;
	}

	Application& Application::AddShutdownFunction(const ApplicationFunction& fn, FunctionSettings threaded)
	{
		m_shutdown_functions.push_back({ fn, threaded});
		return *this;
	}

	void Application::RunStartupFunctions()
	{
		for (const auto& [fn, settings] : m_startup_functions)
		{
			if (settings.threaded)
			{
				m_thread_pool->Enqueue(TaskPriority::HIGH, settings.wait, [this, &fn]() {
						std::unique_lock<std::mutex> lock(m_mutex);
						fn(*this);
					});
			}
			else
			{
				fn(*this);
			}
		}

		m_thread_pool->SyncRegisteredThreads(); // Wait for all threaded startup functions to complete
	}

	void Application::RunUpdateFunctions()
	{
		for (const auto& [fn, settings] : m_update_functions)
		{
			if (settings.threaded)
			{
				m_thread_pool->Enqueue(TaskPriority::NORMAL, settings.wait, [this, &fn]() {
						std::unique_lock<std::mutex> lock(m_mutex);
						fn(*this);
					});
			}
			else
			{
				fn(*this);
			}
		}

		for (const auto& fn : m_query_functions)
		{
			m_thread_pool->Enqueue(TaskPriority::NORMAL, false, [this, &fn]() {
					std::unique_lock<std::mutex> lock(m_mutex);
					fn->Execute(*this);
				});
		}

		m_thread_pool->SyncRegisteredThreads(); // Wait for all threaded update functions to complete
	}

	void Application::RunShutdownFunctions()
	{
		for (const auto& [fn, settings] : m_shutdown_functions)
		{
			if (settings.threaded)
			{
				m_thread_pool->Enqueue(TaskPriority::NORMAL, settings.wait, [this, &fn]() {
						std::unique_lock<std::mutex> lock(m_mutex);
						fn(*this);
					});
			}
			else
			{
				fn(*this);
			}
		}

		m_thread_pool->SyncRegisteredThreads();
	}

	Ecs& Application::GetEcs() const
	{
		return *m_ecs;
	}

	Window& Application::GetWindow()  const
	{	
		return *m_window;
	}

	Renderer& Application::GetRenderer() const
	{
		return *m_renderer;
	}

	EventHandler& Application::GetEventHandler() const
	{
		return *m_event_handler;
	}
}