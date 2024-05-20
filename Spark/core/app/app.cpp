#include "app.hpp"

#include "../events/sub.hpp"
#include "../logging/log.hpp"
#include "../ui/ui.hpp"
#include "../window/window_manager.hpp"
#include "../ecs/ecs.hpp"
#include "app_interface.hpp"

namespace Spark
{
f64 Application::s_fixed_delta_time = 0.005;

f64 Application::s_delta_time = 0;

f64 Application::s_last_frame_time = 0;

f64 Application::s_total_time = 0;

u64 Application::s_tick_speed = 60;

std::unique_ptr<Timer> Application::s_timer = std::make_unique<Timer>();

void Application::on_start()
{
    Application::add_scene("Main Scene", SceneConfig(Math::vec4(0)));
    Subscription<Event>::create(EVERY_EVENT_TOPIC, AppFunctions::s_on_event);

    auto &ecs = Engine::get<ECS>();

    AppFunctions::s_on_start();

    ecs.start_systems();

    s_timer->start();

    auto window = WindowManager::get().get_current_window();

    while (window->is_running())
    {
        on_update();
    }
}

void Application::on_update()
{
    calculate_delta_time();

    AppFunctions::s_on_update();

    auto window = Engine::get<WindowManager>().get_current_window();

    if (window)
        window->on_update();

    Spark::ThreadPool::synchronize_registered_threads();
}

void Application::on_event(std::shared_ptr<Event> event)
{
    ThreadPool::enqueue(TaskPriority::CRITICAL, false, [event]() {
        Spark::EventDispatcher dispatcher(event);
        return dispatcher.dispatch(AppFunctions::s_on_event);
    });
}

void Application::on_shutdown()
{
    auto &ecs = Engine::get<ECS>();

    ecs.shutdown_systems();
}

void Application::set_window_title(const std::string &title)
{
    auto window = Engine::get<WindowManager>().get_current_window();

    if (window)
        window->set_window_title(title);
}

void Application::add_scene(const std::string &name, const SceneConfig &config)
{
    auto &scene_manager = Engine::get<SceneManager>();
    scene_manager.add_scene(name, std::make_unique<Scene>(config));
}

API Application::get_current_api()
{
    return Spark::get_current_api();
}

void Application::set_delta_time(u64 delta_time)
{
    s_delta_time = delta_time;
}

void Application::calculate_delta_time()
{
    // Update last frame time for frame rate calculations
    f64 time = s_timer->elapsed_milliseconds();
    s_last_frame_time = time - s_total_time;
    s_delta_time = s_last_frame_time;
    s_total_time = time;
}

f64 Application::get_delta_time()
{
    return s_last_frame_time / 1000.0; // Convert milliseconds to seconds
}

f64 Application::get_fixed_delta_time()
{
    return s_fixed_delta_time;
}

f64 Application::get_last_frame_time()
{
    return s_last_frame_time;
}

f64 Application::get_total_time()
{
    return s_total_time;
}

u64 Application::get_tick_speed()
{
    return s_tick_speed;
}

void Application::set_fixed_delta_time(f64 time)
{
    s_fixed_delta_time = time;
}

void Application::set_tick_speed(u64 speed)
{
    s_tick_speed = speed;
}

Scene &Application::get_current_scene()
{
    return Engine::get<SceneManager>().get_current_scene();
}
} // namespace Spark