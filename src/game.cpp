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
    spark::material_manager& mat_man = spark::application::get_material_manager();
    spark::mesh_manager& mesh_man = spark::application::get_mesh_manager();
    spark::scene& cur_scene = spark::application::get_current_scene();
    spark::ecs& ecs = spark::application::get_ecs();

    spark::shader_manager& shader_man = spark::application::get_shader_manager();

    auto [vert, frag] = shader_man.load_shader({"Spark/shaders/line.vert", "Spark/shaders/line.frag"});
    SPARK_INFO("Shaders: " << vert);
    cur_scene.set_background_color(spark::math::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    
    std::vector<spark::vertex> vertices = {
        // Position                      // Normal             // Texture Coords
        {spark::math::vec3(0.5f,  0.5f, 0.0f), spark::math::vec3(0.0f, 0.0f, 1.0f), spark::math::vec2(1.0f, 1.0f)},  // Top Right
        {spark::math::vec3(0.5f, -0.5f, 0.0f), spark::math::vec3(0.0f, 0.0f, 1.0f), spark::math::vec2(1.0f, 0.0f)},  // Bottom Right
        {spark::math::vec3(-0.5f, -0.5f, 0.0f), spark::math::vec3(0.0f, 0.0f, 1.0f), spark::math::vec2(0.0f, 0.0f)}, // Bottom Left
        {spark::math::vec3(-0.5f,  0.5f, 0.0f), spark::math::vec3(0.0f, 0.0f, 1.0f), spark::math::vec2(0.0f, 1.0f)}  // Top Left
    };


    // Create material and mesh
    //spark::texture& tex = tex_man.create_texture("tex", "assets/sprites/grass.jpg", spark::texture_type::TWO_D);
    spark::material& mat = mat_man.create_material("material", {}, spark::math::vec4(0.0f, 1.0f, 1.0f, 1.0f));
    spark::instanced_mesh& mesh = mesh_man.create_mesh("square", vertices, {0, 1, 3, 1, 2, 3});

	// Create entity
    
    for (int i = 0; i < 10; i++)
    {
        spark::transform_component trans = spark::transform_component(spark::math::vec3(i, i, i));

        spark::entity ent = ecs.create_entity(
            trans,
            spark::mesh_component("square"),
            spark::material_component("material")
        );

        spark::transform_component trans2 = spark::transform_component(spark::math::vec3(-i, -i, -i));

        spark::entity ent2 = ecs.create_entity(
            trans2,
            spark::mesh_component("square"),
            spark::material_component("material")
        );

        renderer.get_instancer().add_renderable(cur_scene, "square", "material", trans);
        renderer.get_instancer().add_renderable(cur_scene, "square", "material", trans2);
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
    x += 0.001f;

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