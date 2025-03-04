#include "spark_pch.hpp"

#include "spark_application.hpp"
#include "spark_item.hpp"
#include "spark_render_system.hpp"

namespace spark
{
	Application::Application(GraphicsApi gapi,
		const std::string& title,
		int32_t win_width,
		int32_t win_height,
		bool vsync)
		: m_gapi(gapi)
		, m_mod_manager(this)
		, m_item_manager(std::make_unique<ItemManager>(this))
	{
		// Basic layering
		m_layer_stack.PushLayer<WindowLayer>(m_event_queue, gapi, title, win_width, win_height, vsync);
		m_layer_stack.PushLayer<RendererLayer>(m_coordinator, gapi, m_command_queue);
		m_layer_stack.PushLayer<EventLayer>(*this, m_event_queue);

		RenderSystem render_system(m_coordinator, m_command_queue);

		RegisterSystem(
		[this, &render_system](Query<RenderableComponent> query)
		{
			render_system.Update(std::move(query));
		});

		// Add these resources
		AddResource(*this);
		AddResource(m_coordinator);
		AddResource(m_thread_pool);
		AddResource(m_delta_time);
		AddResource(m_gapi);
		AddResource(m_layer_stack);
		AddResource(m_command_queue);
		AddResource(m_item_manager);
		AddResource(m_event_queue);

		SetupFactories();
	}

	void Application::ClearItems()
	{
		m_item_manager->Clear();
	}
}