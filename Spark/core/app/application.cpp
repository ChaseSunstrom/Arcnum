#include "application.hpp"

namespace Spark
{
	Application::~Application()
	{
		RunShutdownFunctions();
	}

	void Application::Start()
	{
		RunStartupFunctions();

		while (true)
		{
			Update();
		}
	}

	void Application::Update()
	{
		RunUpdateFunctions();
		m_window->Update();
	}

	Application& Application::AddStartupFunction(ApplicationFunction fn)
	{
		m_startup_functions.push_back(fn);
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

	Ecs& Application::GetEcs()
	{
		return *m_ecs;
	}
}