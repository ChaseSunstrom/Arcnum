#include "core_include.hpp"
#include "game.hpp"

#include <core/net/chat_message.hpp>

void on_start()
{
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

    spark::material_manager& mat_man = spark::application::get_material_manager();
    spark::mesh_manager& mesh_man = spark::application::get_mesh_manager();
    spark::scene& cur_scene = spark::application::get_current_scene();
    spark::ecs& ecs = spark::application::get_ecs();


    cur_scene.set_background_color(spark::math::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    
    std::vector<spark::vertex> vertices = {
         spark::math::vec3(0.5f,  0.5f, 0.0f),  // top right
         spark::math::vec3(0.5f, -0.5f, 0.0f),  // bottom right
         spark::math::vec3(-0.5f, -0.5f, 0.0f),  // bottom left
         spark::math::vec3(-0.5f,  0.5f, 0.0f)   // top left
    };

    // Create material and mesh
    spark::material& mat = mat_man.create_material("material", {}, spark::math::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    spark::instanced_mesh& mesh = mesh_man.create_mesh("square", vertices, {0, 1, 3, 1, 2, 3});

	// Create entity
    spark::entity ent = ecs.create_entity(
        spark::transform_component(spark::math::vec3(0.0f, 0.0f, 0.0f)),
        spark::mesh_component("square"),
		spark::material_component("material")
    );
}

void on_update()
{
	
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