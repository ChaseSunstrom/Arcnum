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

	Application& Application::AddStartupFunction(ApplicationFunction fn)
	{
		m_startup_functions.push_back(fn);
		return *this;
	}

	Application& Application::AddEventFunction(i64 event_type, ApplicationEventFunction fn)
	{
		m_event_functions.push_back(fn);
		m_event_handler->SubscribeToEvent(event_type, [this, fn](const std::shared_ptr<Event> event) {
			fn(*this, event);
			});
		return *this;
	}

	Application& Application::AddUpdateFunction(ApplicationFunction fn)
	{
		m_update_functions.push_back(fn);
		return *this;
	}

	Application& Application::AddShutdownFunction(ApplicationFunction fn)
	{
		m_shutdown_functions.push_back(fn);
		return *this;
	}

	void Application::RunStartupFunctions()
	{
		for (auto& fn : m_startup_functions)
		{
			fn(*this);
		}
	}

	void Application::RunUpdateFunctions()
	{
		for (const auto& fn : m_update_functions)
		{
			fn(*this);
		}

		for (const auto& fn : m_query_functions)
		{
			fn->Execute(*this);
		}
	}

	void Application::RunShutdownFunctions()
	{
		for (auto& fn : m_shutdown_functions)
		{
			fn(*this);
		}
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