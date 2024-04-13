#include "core_include.hpp"
#include "game.hpp"

#include <core/net/chat_message.hpp>

void add_sphere_entity(const spark::math::vec3& position)
{
	spark::f32 clamped_scale = std::clamp(position.x, 0.1f, 1.0f);

	spark::create_shape<spark::sphere>("material", position, position, spark::math::vec3(clamped_scale, clamped_scale, clamped_scale), 64, 32);
}

void add_cube_entity(const spark::math::vec3& position)
{
	spark::f32 clamped_scale = std::clamp(position.x, 0.1f, 1.0f);

	spark::create_shape<spark::cube>("material", position, position, spark::math::vec3(clamped_scale, clamped_scale, clamped_scale));
}

static std::shared_ptr<spark::net::udp_server> g_server_instance;
static std::shared_ptr<spark::net::udp_client> g_client_instance;

void setup_server_ui(spark::window_component& server_window) {
	static char server_ip[128] = "127.0.0.1";
	static char server_port[6] = "8080";

	server_window.add_child(std::make_shared<spark::text_input_component>("ServerIP", "Server IP", server_ip, sizeof(server_ip)));
	server_window.add_child(std::make_shared<spark::text_input_component>("ServerPort", "Server Port", server_port, sizeof(server_port)));

	server_window.add_child(std::make_shared<spark::button_component>("HostServer", "Host Server", [&]()
		{
			if (!g_server_instance)
			{
				g_server_instance = std::make_shared<spark::net::udp_server>(server_ip, server_port);
				spark::thread_pool::enqueue(spark::task_priority::HIGH, false, []()
					{
						g_server_instance->run();
					});
			}
		}));
}

void setup_client_ui(spark::window_component& client_window) {
	static char client_ip[128] = "127.0.0.1";
	static char client_port[6] = "8080";
	static char message_buffer[1024] = "";

	client_window.add_child(std::make_shared<spark::text_input_component>("ClientIP", "Server IP", client_ip, sizeof(client_ip)));
	client_window.add_child(std::make_shared<spark::text_input_component>("ClientPort", "Server Port", client_port, sizeof(client_port)));
	client_window.add_child(std::make_shared<spark::text_input_component>("Message", "Message", message_buffer, sizeof(message_buffer)));

	client_window.add_child(std::make_shared<spark::button_component>("SendMessage", "Send Message", [&]()
		{
			if (g_client_instance)
			{
				spark::net::chat_message msg(message_buffer);
				g_client_instance->send(msg);
				memset(message_buffer, 0, sizeof(message_buffer)); // Clear the message buffer after sending
			}
		}));

	client_window.add_child(std::make_shared<spark::button_component>("Connect", "Connect", [&]()
		{
			if (!g_client_instance)
			{
				g_client_instance = std::make_shared<spark::net::udp_client>(client_ip, client_port);
				spark::thread_pool::enqueue(spark::task_priority::HIGH, false, []()
					{
						g_client_instance->run();
					});
			}
		}));
}


void on_start()
{
	auto& _ui_manager = spark::engine::get<spark::ui_manager>();
	auto& _audio_manager = spark::engine::get<spark::audio_manager>();
	auto& _material_manager = spark::engine::get<spark::material_manager>();
	auto& _mesh_manager = spark::engine::get<spark::mesh_manager>();
	auto& _scene_manager = spark::engine::get<spark::scene_manager>();
	auto& _shader_manager = spark::engine::get<spark::shader_manager>();
	auto& _ecs = spark::engine::get<spark::ecs>();

	std::cout << "SIZE OF THEME: " << sizeof(spark::ui_theme) << std::endl;

	spark::application::set_window_title("Arcnum");

	// Setup the window for server controls
	auto server_window = std::make_unique<spark::window_component>("ServerWindow", "Server Controls");
	auto& ref_server_window = *server_window;
	_ui_manager.add_window(std::move(server_window));
	setup_server_ui(ref_server_window);

	// Setup the window for client controls
	auto client_window = std::make_unique<spark::window_component>("ClientWindow", "Client Controls");
	auto& ref_client_window = *client_window;
	_ui_manager.add_window(std::move(client_window));
	setup_client_ui(ref_client_window);

	_ui_manager.create_component<spark::multi_text_component>("", "Text", std::vector<std::string>{ });

	spark::scene& cur_scene = _scene_manager.get_current_scene();

	_audio_manager.create_sound("retro", "assets/sfx/retro.wav");

	cur_scene.set_background_color(spark::math::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	// Create material and mesh
	spark::material& material = _material_manager.create_material("material", {}, spark::math::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	std::default_random_engine random_engine;
	std::uniform_real_distribution<spark::f32> distribution(0.0f, 1.0f);

	// Create entity

	_ui_manager.create_component<spark::button_component>("", "Spawn Sphere", "Spawn Sphere", std::function<void()>([]()
		{
			static std::random_device rd; // Obtain a random number from hardware
			static std::mt19937 eng(rd()); // Seed the generator
			static std::uniform_real_distribution<> distr(-3.0, 3.0); // Define the range

			// Generate a random position
			spark::math::vec3 random_position(distr(eng), distr(eng), distr(eng));

			// Assuming a function or mechanism to add a cube entity
			add_sphere_entity(random_position);
		}));

	_ui_manager.create_component<spark::button_component>("", "Spawn Cube", "Spawn Cube", std::function<void()>([]()
		{
			static std::random_device rd; // Obtain a random number from hardware
			static std::mt19937 eng(rd()); // Seed the generator
			static std::uniform_real_distribution<> distr(-3.0, 3.0); // Define the range

			// Generate a random position
			spark::math::vec3 random_position(distr(eng), distr(eng), distr(eng));

			// Assuming a function or mechanism to add a cube entity
			add_cube_entity(random_position);
		}));

	spark::create_shape<spark::square>("material", spark::math::vec3(0.0f, 0.0f, 0.0f), spark::math::vec3(0.0f, 0.0f, 0.0f), spark::math::vec3(1.0f, 1.0f, 1.0f));
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
	static spark::f32 x = 3; // Static to keep its value between calls
	static spark::f32 time = 0.0f;

	auto& _renderer = spark::engine::get<spark::renderer>();
	auto& _material = spark::engine::get<spark::material_manager>().get_material("material");

	update_material_color(_material, time);

	spark::camera& camera = *_renderer.get_cameras()[0];

	camera.m_position = spark::math::vec3(0.0f, 0.0f, x);
	x += 0.0001f;
	time += 0.001f;
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