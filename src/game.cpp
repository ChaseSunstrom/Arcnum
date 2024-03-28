#include "core_include.hpp"
#include "game.hpp"

#include <core/net/chat_message.hpp>


std::vector<spark::vertex> vertices = {
		spark::vertex(spark::math::vec3(-0.5f, -0.5f, -0.5f)),
		spark::vertex(spark::math::vec3(0.5f, -0.5f, -0.5f)),
		spark::vertex(spark::math::vec3(0.5f,  0.5f, -0.5f)),
		spark::vertex(spark::math::vec3(0.5f,  0.5f, -0.5f)),
		spark::vertex(spark::math::vec3(-0.5f,  0.5f, -0.5f)),
		spark::vertex(spark::math::vec3(-0.5f, -0.5f, -0.5f)),
		spark::vertex(spark::math::vec3(-0.5f, -0.5f,  0.5f)),
		spark::vertex(spark::math::vec3(0.5f, -0.5f,  0.5f)),
		spark::vertex(spark::math::vec3(0.5f,  0.5f,  0.5f)),
		spark::vertex(spark::math::vec3(0.5f,  0.5f,  0.5f)),
		spark::vertex(spark::math::vec3(-0.5f,  0.5f,  0.5f)),
		spark::vertex(spark::math::vec3(-0.5f, -0.5f,  0.5f)),
		spark::vertex(spark::math::vec3(-0.5f,  0.5f,  0.5f)),
		spark::vertex(spark::math::vec3(-0.5f,  0.5f, -0.5f)),
		spark::vertex(spark::math::vec3(-0.5f, -0.5f, -0.5f)),
		spark::vertex(spark::math::vec3(-0.5f, -0.5f, -0.5f)),
		spark::vertex(spark::math::vec3(-0.5f, -0.5f,  0.5f)),
		spark::vertex(spark::math::vec3(-0.5f,  0.5f,  0.5f)),
		spark::vertex(spark::math::vec3(0.5f,  0.5f,  0.5f)),
		spark::vertex(spark::math::vec3(0.5f,  0.5f, -0.5f)),
		spark::vertex(spark::math::vec3(0.5f, -0.5f, -0.5f)),
		spark::vertex(spark::math::vec3(0.5f, -0.5f, -0.5f)),
		spark::vertex(spark::math::vec3(0.5f, -0.5f,  0.5f)),
		spark::vertex(spark::math::vec3(0.5f,  0.5f,  0.5f)),
		spark::vertex(spark::math::vec3(-0.5f, -0.5f, -0.5f)),
		spark::vertex(spark::math::vec3(0.5f, -0.5f, -0.5f)),
		spark::vertex(spark::math::vec3(0.5f, -0.5f,  0.5f)),
		spark::vertex(spark::math::vec3(0.5f, -0.5f,  0.5f)),
		spark::vertex(spark::math::vec3(-0.5f, -0.5f,  0.5f)),
		spark::vertex(spark::math::vec3(-0.5f, -0.5f, -0.5f)),
		spark::vertex(spark::math::vec3(-0.5f,  0.5f, -0.5f)),
		spark::vertex(spark::math::vec3(0.5f,  0.5f, -0.5f)),
		spark::vertex(spark::math::vec3(0.5f,  0.5f,  0.5f)),
		spark::vertex(spark::math::vec3(0.5f,  0.5f,  0.5f)),
		spark::vertex(spark::math::vec3(-0.5f,  0.5f,  0.5f)),
		spark::vertex(spark::math::vec3(-0.5f,  0.5f, -0.5f))
};

void add_cube_entity(const spark::math::vec3& position)
{
	auto& _ecs = spark::engine::get<spark::ecs>();
	auto& _mesh_manager = spark::engine::get<spark::mesh_manager>();
	auto& _material_manager = spark::engine::get<spark::material_manager>();
	auto& _renderer = spark::engine::get<spark::renderer>();
	auto& _scene_manager = spark::engine::get<spark::scene_manager>();
	spark::scene& cur_scene = _scene_manager.get_current_scene();

	
	std::string materialName = "material";

	spark::transform_component trans = spark::transform_component(position);
	spark::entity ent = _ecs.create_entity(
		trans,
		spark::mesh_component("square"),
		spark::material_component(materialName) 
	);

	_renderer.get_instancer().add_renderable(cur_scene, "square", materialName, trans);
}

void on_start()
{
	static char message_buffer[1024] = "";

	auto& _ui_manager = spark::engine::get<spark::ui_manager>();

	std::cout << "SIZE OF THEME: " << sizeof(spark::ui_theme) << std::endl;

	spark::application::set_window_title("Arcnum");

	spark::thread_pool::enqueue(spark::task_priority::HIGH, false, []()
								{
									spark::net::udp_server server("127.0.0.1", "8080");
									server.run();
								});


	auto client = std::make_shared<spark::net::udp_client>("127.0.0.1", "8080");

	spark::thread_pool::enqueue(spark::task_priority::HIGH, false, [client]()
		{
			client->run();
		});

	_ui_manager.create_component<spark::text_input_component>("", "Message", "Message", message_buffer, sizeof(message_buffer));
	_ui_manager.create_component<spark::multi_text_component>("", "Text", std::vector<std::string>{ "Text" });

	_ui_manager.create_component<spark::button_component>("", "Send", "Send", [client]()
		{
			std::string message(message_buffer); // Convert buffer to std::string
			if (!message.empty())
			{
				spark::net::chat_message msg(message);
				client->send(msg); // Use pointer dereference to access member

				memset(message_buffer, 0, sizeof(message_buffer));
			}
		});


	auto& _renderer = spark::engine::get<spark::renderer>();
	auto& _audio_manager = spark::engine::get<spark::audio_manager>();
	auto& _material_manager = spark::engine::get<spark::material_manager>();
	auto& _mesh_manager = spark::engine::get<spark::mesh_manager>();
	auto& _component_manager = spark::engine::get<spark::component_manager>();
	auto& _scene_manager = spark::engine::get<spark::scene_manager>();
	auto& _shader_manager = spark::engine::get<spark::shader_manager>();
	auto& _ecs = spark::engine::get<spark::ecs>();

	spark::scene& cur_scene = _scene_manager.get_current_scene();

	_audio_manager.create_sound("retro", "assets/sfx/retro.wav");


	auto [vert, frag] = _shader_manager.load_shader({ "Spark/shaders/line.vert", "Spark/shaders/line.frag" });
	SPARK_INFO("Shaders: " << vert);
	cur_scene.set_background_color(spark::math::vec4(1.0f, 0.0f, 0.0f, 1.0f));



	// Create material and mesh
	spark::texture& grass = _material_manager.create_texture("grass", "assets/sprites/grass.jpg", spark::texture_type::TWO_D);
	spark::texture& sand = _material_manager.create_texture("sand", "assets/sprites/sand.jpeg", spark::texture_type::TWO_D);
	spark::material& material = _material_manager.create_material("material", {}, spark::math::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	spark::mesh& mesh = _mesh_manager.create_mesh("square", vertices);
	std::default_random_engine random_engine; 
	std::uniform_real_distribution<spark::float32_t> distribution(0.0f, 1.0f);

	// Create entity

	_ui_manager.create_component<spark::button_component>("", "Spawn Cube", "Spawn", std::function<void()>([]()
		{
			static std::random_device rd; // Obtain a random number from hardware
			static std::mt19937 eng(rd()); // Seed the generator
			static std::uniform_real_distribution<> distr(-3.0, 3.0); // Define the range

			// Generate a random position
			spark::math::vec3 random_position(distr(eng), distr(eng), distr(eng));

			// Assuming a function or mechanism to add a cube entity
			add_cube_entity(random_position);
		}));

}

void on_update()
{
	static float x = 3; // Static to keep its value between calls

	auto& _renderer = spark::engine::get<spark::renderer>();
	
	spark::camera& camera = *_renderer.get_cameras()[0];

	camera.m_position = spark::math::vec3(0.0f, 0.0f, x);
	x += 0.0001f;
}

// required function
// will recieve events from everything, it is automatically subscribed to recieve
// every event
bool on_event(std::shared_ptr<spark::event> event)
{
	switch (event->m_type)
	{
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
	}
	return true;
}

// Registers functions to be called at different points in the application
// automatically
void register_functions()
{
	spark::app_functions::register_functions(on_start, on_update, on_event);
}