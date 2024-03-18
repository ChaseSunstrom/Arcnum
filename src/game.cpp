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