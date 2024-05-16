#include "game.hpp"
#include "core_include.hpp"
#include <core/asset/asset.hpp>
#include <core/net/chat_message.hpp>

struct alignas(16) ubo
{
    Spark::Math::mat4 m_proj;
    Spark::Math::mat4 m_view;
    Spark::Math::mat4 m_model;
};

void on_start()
{
    Spark::set_api(Spark::API::DIRECTX);
    auto &ecs = Spark::Engine::get<Spark::ECS>();

    Spark::Application::set_window_title("Arcnum");

    Spark::Scene &cur_scene = Spark::Application::get_current_scene();

    Spark::AssetManager::create_asset<Spark::Audio>("retro", "assets/sfx/retro.wav");

    cur_scene.set_background_color(Spark::Math::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    // Create material and mesh
    Spark::Material &material =
        Spark::AssetManager::create_asset<Spark::Material>("material", Spark::Math::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    std::default_random_engine random_engine;
    std::uniform_real_distribution<Spark::f32> distribution(0.0f, 1.0f);

    std::vector<Spark::Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f}}, {{0.5f, -0.5f, 0.0f}}, {{0.5f, 0.5f, 0.0f}}, {{-0.5f, 0.5f, 0.0f}}};

    std::vector<Spark::u32> indices = {0, 1, 3, 1, 2, 3};

    ubo _ubo{};
    _ubo.m_model = Spark::Math::mat4(1.0f);
    _ubo.m_proj = Spark::Math::perspective(45.0f, 1.0f, 0.1f, 100.0f);
    _ubo.m_view = Spark::Math::lookAt(Spark::Math::vec3(0.0f, 0.0f, 3.0f), Spark::Math::vec3(0.0f, 0.0f, 0.0f),
                                      Spark::Math::vec3(0.0f, 1.0f, 0.0f));

    Spark::AssetManager::create_asset<Spark::Mesh>("quad", vertices, indices, _ubo);

    Spark::Entity e = ecs.create_entity(Spark::MeshComponent("quad"), Spark::MaterialComponent("material"),
                                        Spark::TransformComponent());
}

void on_update()
{
}

// required function
// will recieve events from everything, it is automatically subscribed to
// recieve every event
bool on_event(std::shared_ptr<Spark::Event> event)
{
    switch (event->m_type)
    {
    case KEY_PRESSED_EVENT: {
        auto received_event = std::dynamic_pointer_cast<Spark::KeyPressedEvent>(event);

        if (received_event)
        {
            SPARK_TRACE("[KEY PRESSED]: " << Spark::to_string(Spark::Key(received_event->m_key_code)));
        }
    }
    break;
    case KEY_REPEAT_EVENT: {
        auto received_event = std::dynamic_pointer_cast<Spark::KeyRepeatedEvent>(event);

        if (received_event)
        {
            SPARK_TRACE("[KEY HELD]: " << Spark::to_string(Spark::Key(received_event->m_key_code)));
        }
    }
    break;
    case KEY_RELEASED_EVENT: {
        auto received_event = std::dynamic_pointer_cast<Spark::KeyReleasedEvent>(event);

        if (received_event)
        {
            SPARK_TRACE("[KEY RELEASED]: " << Spark::to_string(Spark::Key(received_event->m_key_code)));
        }
    }
    break;
    case UDP_SERVER_RECEIVE_EVENT: {
        auto &_ui_manager = Spark::Engine::get<Spark::UIManager>();

        auto received_event = std::dynamic_pointer_cast<Spark::net::UDPServerReceiveEvent>(event);
        if (received_event)
        {
            std::string message = dynamic_cast<Spark::net::ChatMessage *>(&*received_event->m_packet)
                                      ->m_message; // Convert packet to string

            // Find the text component meant to display messages
            auto text_comp = _ui_manager.find_component_by_id<Spark::MultiTextComponent>("Text");
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
    Spark::AppFunctions::register_functions(on_start, on_update, on_event);
}