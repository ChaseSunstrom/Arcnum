#include "core_include.hpp"
#include "game.hpp"

#include <core/net/chat_message.hpp>

struct alignas(16) ubo
{
	spark::math::mat4 m_proj;
	spark::math::mat4 m_view;
	spark::math::mat4 m_model;
};

void on_start()
{
	auto& _ui_manager = spark::engine::get<spark::ui_manager>();
	auto& _audio_manager = spark::engine::get<spark::audio_manager>();
	auto& _material_manager = spark::engine::get<spark::material_manager>();
	auto& _mesh_manager = spark::engine::get<spark::mesh_manager>();
	auto& _scene_manager = spark::engine::get<spark::scene_manager>();
	auto& _shader_manager = spark::engine::get<spark::shader_manager>();
	auto& _ecs = spark::engine::get<spark::ecs>();

	spark::application::set_window_title("Arcnum");

	spark::scene& cur_scene = _scene_manager.get_current_scene();

	_audio_manager.create_sound("retro", "assets/sfx/retro.wav");

	cur_scene.set_background_color(spark::math::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	// Create material and mesh
	spark::material& material = _material_manager.create_material("material", {}, spark::math::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	std::default_random_engine random_engine;
	std::uniform_real_distribution<spark::f32> distribution(0.0f, 1.0f);


	std::vector<spark::vertex> vertices = {
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

	_mesh_manager.create_mesh("quad", vertices, indices, _ubo);

	spark::entity e = _ecs.create_entity(
		spark::mesh_component("quad"),
		spark::material_component("material"),
		spark::transform_component()
	);
}

void update_material_color(spark::material& mat, spark::f32 time)
{
	spark::f32 new_time = std::clamp(time, 1.0f, 255.0f); // Keep the time between 0 and 255
	// Use the sine function to get a value between 0 and 1 for R, G, and B colors, creating a smooth transition
	spark::f32 red = (std::sin(new_time * 0.6f) + 1.0f) / 2.0f; // Vary the multiplier for speed of color change
	spark::f32 green = (std::sin(new_time * 0.7f) + 1.0f) / 2.0f;
	spark::f32 blue = (std::sin(new_time * 0.5f) + 1.0f) / 2.0f;

	// Set the new color of the material
	mat.m_color = spark::math::vec4(red, green, blue, 1.0f); // Alpha is set to 1 for full opacity
}

void on_update()
{
}

// required function
// will recieve events from everything, it is automatically subscribed to recieve
// every event
bool on_event(std::shared_ptr<spark::event> event)
{
	switch (event->m_type)
	{
	case KEY_PRESSED_EVENT:
	{
		auto received_event = std::dynamic_pointer_cast<spark::key_pressed_event>(event);

		if (received_event)
		{
			SPARK_TRACE("[KEY PRESSED]: " << spark::to_string(spark::key(received_event->m_key_code)));
		}
	}
	break;
	case KEY_REPEAT_EVENT:
	{
		auto received_event = std::dynamic_pointer_cast<spark::key_repeat_event>(event);

		if (received_event)
		{
			SPARK_TRACE("[KEY HELD]: " << spark::to_string(spark::key(received_event->m_key_code)));
		}
	}
	break;
	case KEY_RELEASED_EVENT:
	{
		auto received_event = std::dynamic_pointer_cast<spark::key_released_event>(event);

		if (received_event)
		{
			SPARK_TRACE("[KEY RELEASED]: " << spark::to_string(spark::key(received_event->m_key_code)));
		}
	}
	break;
	case UDP_SERVER_RECEIVE_EVENT:
	{
		auto& _ui_manager = spark::engine::get<spark::ui_manager>();

		auto received_event = std::dynamic_pointer_cast<spark::net::udp_server_receive_event>(event);
		if (received_event)
		{
			std::string message = dynamic_cast<spark::net::chat_message*>(&*received_event->m_packet)->m_message; // Convert packet to string

			// Find the text component meant to display messages
			auto text_comp = _ui_manager.find_component_by_id<spark::multi_text_component>("Text");
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
	spark::app_functions::register_functions(on_start, on_update, on_event);
}