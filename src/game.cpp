#include "core_include.hpp"
#include "game.hpp"

#include <core/asset/asset.hpp>
#include <core/net/chat_message.hpp>

struct alignas(16) ubo
{
	spark::math::mat4 m_proj;
	spark::math::mat4 m_view;
	spark::math::mat4 m_model;
};

void on_start()
{
	auto& ecs = spark::Engine::get<spark::ECS>();

	spark::Application::set_window_title("Arcnum");

	spark::Scene& cur_scene = spark::Application::get_current_scene();

	spark::AssetManager::create_asset<spark::Sound>("retro", "assets/sfx/retro.wav");

	cur_scene.set_background_color(spark::math::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	// Create material and mesh
	spark::Material& material = spark::AssetManager::create_asset<spark::Material>("material", {}, spark::math::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	std::default_random_engine random_engine;
	std::uniform_real_distribution<spark::f32> distribution(0.0f, 1.0f);

	std::vector<spark::Vertex> vertices = {
		{ { -0.5f, -0.5f, 0.0f }},
		{ { 0.5f, -0.5f, 0.0f }},
		{ { 0.5f, 0.5f, 0.0f }},
		{ { -0.5f, 0.5f, 0.0f }}
	};

	std::vector<spark::u32> indices = {
		0, 1, 3,
		1, 2, 3
	};

	ubo _ubo{};
	_ubo.m_model = spark::math::mat4(1.0f);
	_ubo.m_proj = spark::math::perspective(45.0f, 1.0f, 0.1f, 100.0f);
	_ubo.m_view = spark::math::lookAt(spark::math::vec3(0.0f, 0.0f, 3.0f), spark::math::vec3(0.0f, 0.0f, 0.0f), spark::math::vec3(0.0f, 1.0f, 0.0f));
	
	spark::AssetManager::create_asset<spark::Mesh>("quad", vertices, indices, _ubo);

	spark::Entity e = ecs.create_entity(
		spark::MeshComponent("quad"),
		spark::MaterialComponent("material"),
		spark::TransformComponent()
	);
}

void on_update()
{

}

// required function
// will recieve events from everything, it is automatically subscribed to recieve
// every event
bool on_event(std::shared_ptr<spark::Event> event)
{
	switch (event->m_type)
	{
	case KEY_PRESSED_EVENT:
	{
		auto received_event = std::dynamic_pointer_cast<spark::KeyPressedEvent>(event);

		if (received_event)
		{
			SPARK_TRACE("[KEY PRESSED]: " << spark::to_string(spark::Key(received_event->m_key_code)));
		}
	}
	break;
	case KEY_REPEAT_EVENT:
	{
		auto received_event = std::dynamic_pointer_cast<spark::KeyRepeatedEvent>(event);

		if (received_event)
		{
			SPARK_TRACE("[KEY HELD]: " << spark::to_string(spark::Key(received_event->m_key_code)));
		}
	}
	break;
	case KEY_RELEASED_EVENT:
	{
		auto received_event = std::dynamic_pointer_cast<spark::KeyReleasedEvent>(event);

		if (received_event)
		{
			SPARK_TRACE("[KEY RELEASED]: " << spark::to_string(spark::Key(received_event->m_key_code)));
		}
	}
	break;
	case UDP_SERVER_RECEIVE_EVENT:
	{
		auto& _ui_manager = spark::Engine::get<spark::UIManager>();

		auto received_event = std::dynamic_pointer_cast<spark::net::UDPServerReceiveEvent>(event);
		if (received_event)
		{
			std::string message = dynamic_cast<spark::net::ChatMessage*>(&*received_event->m_packet)->m_message; // Convert packet to string

			// Find the text component meant to display messages
			auto text_comp = _ui_manager.find_component_by_id<spark::MultiTextComponent>("Text");
			if (text_comp)
			{
				text_comp->add_text(message); // Update the text component with the new message
			}
		}
	}
	break;
	}
	return true;
}

// Registers functions to be called at different points in the application
// automatically
void register_functions()
{
	spark::AppFunctions::register_functions(on_start, on_update, on_event);
}