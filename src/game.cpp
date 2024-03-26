#include "core_include.hpp"
#include "game.hpp"

#include <core/net/chat_message.hpp>

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

	spark::renderer& renderer = spark::application::get_renderer();

	spark::texture_manager& tex_man = spark::application::get_texture_manager();
	spark::audio_manager& audio_man = spark::application::get_audio_manager();
	spark::material_manager& mat_man = spark::application::get_material_manager();
	spark::mesh_manager& mesh_man = spark::application::get_mesh_manager();

	spark::scene& cur_scene = spark::application::get_current_scene();
	audio_man.create_sound("retro", "assets/sfx/retro.wav");

	spark::ecs& ecs = spark::application::get_ecs();
	spark::component_manager& comp_man = ecs.get_component_manager();




	spark::shader_manager& shader_man = spark::application::get_shader_manager();

	auto [vert, frag] = shader_man.load_shader({ "Spark/shaders/line.vert", "Spark/shaders/line.frag" });
	SPARK_INFO("Shaders: " << vert);
	cur_scene.set_background_color(spark::math::vec4(1.0f, 0.0f, 0.0f, 1.0f));

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


	// Create material and mesh
	//spark::texture& tex = tex_man.create_texture("tex", "assets/sprites/grass.jpg", spark::texture_type::TWO_D);
	spark::instanced_mesh& mesh = mesh_man.create_mesh("square", vertices);
	std::default_random_engine random_engine; // Consider seeding with a variable for reproducibility
	std::uniform_real_distribution<spark::float32_t> distribution(0.0f, 1.0f);

	// Create entity

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			for (int k = 0; k < 5; k++)
			{
				std::string name = "material" + std::to_string(i + j + k);
				// Generate random colors for the rainbow effect
				spark::math::vec4 random_color(distribution(random_engine), distribution(random_engine), distribution(random_engine), 1.0f);
				spark::material& mat = mat_man.create_material(name, {}, random_color);

				spark::transform_component trans = spark::transform_component(spark::math::vec3(std::sin(i) * distribution(random_engine) * 2, std::cos(k) * 2 * distribution(random_engine), std::tan(j) * 2 * distribution(random_engine)) * distribution(random_engine));

				spark::entity ent = ecs.create_entity(
					trans,
					spark::mesh_component("square"),
					spark::material_component(name), // Ensure to reference the correct material name
					spark::audio_component("retro", [](spark::component_manager&) -> bool
										   {
											   return true;
										   })
				);

				renderer.get_instancer().add_renderable(cur_scene, "square", name, trans);
			}
		}
	}
}

void on_update()
{
	static float x = 3; // Static to keep its value between calls

	spark::scene& cur_scene = spark::application::get_current_scene();
	spark::material_manager& mat_man = spark::application::get_material_manager();
	spark::renderer& renderer = spark::application::get_renderer();
	spark::camera& camera = *renderer.get_cameras()[0];

	camera.m_position = spark::math::vec3(0.0f, 0.0f, x);
	x += 0.1f;
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