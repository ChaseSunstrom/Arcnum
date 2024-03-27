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
		spark::mesh_component("square"), // Assuming "square" mesh exists
		spark::material_component(materialName) // Assumes material exists
	);

	// Assuming the renderer and scene setup allows adding renderables like this
	_renderer.get_instancer().add_renderable(cur_scene, "square", materialName, trans);
}

void on_start()
{
	spark::application::set_window_title("Arcnum");

	spark::thread_pool::enqueue(spark::task_priority::HIGH, false, []()
								{
									spark::net::udp_server server("127.0.0.1", "8080");
									server.run();
								});


	spark::thread_pool::enqueue(spark::task_priority::HIGH, false, []()
								{
									spark::net::udp_client client("127.0.0.1", "8080");
									// Needs to be ran in a seprate thread
									spark::thread_pool::enqueue(spark::task_priority::HIGH, false, [&client]()
																{
																	client.run();
																});

									std::string line;
									while (std::getline(std::cin, line))
									{
										spark::net::chat_message msg(line);
										client.send(msg);
									}
								});

	auto& _renderer = spark::engine::get<spark::renderer>();
	auto& _audio_manager = spark::engine::get<spark::audio_manager>();
	auto& _material_manager = spark::engine::get<spark::material_manager>();
	auto& _mesh_manager = spark::engine::get<spark::mesh_manager>();
	auto& _component_manager = spark::engine::get<spark::component_manager>();
	auto& _scene_manager = spark::engine::get<spark::scene_manager>();
	auto& _ui_manager = spark::engine::get<spark::ui_manager>();
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

	_ui_manager.create_component<spark::button_component>("", "Spawn Cube", []()
														 {
															 static std::random_device rd; // Obtain a random number from hardware
															 static std::mt19937 eng(rd()); // Seed the generator
															 static std::uniform_real_distribution<> distr(-3.0, 3.0); // Define the range

															 // Generate a random position
															 spark::math::vec3 random_position(distr(eng), distr(eng), distr(eng));

															 // Assuming a function or mechanism to add a cube entity
															 add_cube_entity(random_position);
														 });
}

void on_update()
{
	static float x = 3; // Static to keep its value between calls

	auto& _renderer = spark::engine::get<spark::renderer>();
	
	spark::camera& camera = *_renderer.get_cameras()[0];

	camera.m_position = spark::math::vec3(0.0f, 0.0f, x);
	x += 0.01f;
}

// required function
// will recieve events from everything, it is automatically subscribed to recieve
// every event
bool on_event(std::shared_ptr<spark::event> event)
{
	SPARK_INFO(event->m_type);
	return true;
}

// Registers functions to be called at different points in the application
// automatically
void register_functions()
{
	spark::app_functions::register_functions(on_start, on_update, on_event);
}