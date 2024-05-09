#include "app.hpp"

#include "../events/sub.hpp"
#include "../logging/log.hpp"
#include "../ui/ui.hpp"
#include "../window/window_manager.hpp"
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
    Application::add_scene("Main Scene", SceneConfig(math::vec4(0)));

    auto &_window_man = WindowManager::get();
    auto &_window = _window_man.get_current_window();
    auto &_ecs = Engine::get<ECS>();
    auto &_ui = Engine::get<UIManager>();

    AppFunctions::s_on_start();

    _ecs.start_systems();

    Subscription<Event>::create(EVERY_EVENT_TOPIC, AppFunctions::s_on_event);

    s_timer->start();

    while (_window.is_running())
    {
        on_update();
    }
}

void Application::on_update()
{
    calculate_delta_time();

    AppFunctions::s_on_update();

    auto &_window = Engine::get<WindowManager>().get_current_window();
    auto &_renderer = Engine::get<Renderer>();
    auto &_scene_manager = Engine::get<SceneManager>();
    auto &_ecs = Engine::get<ECS>();
    auto &_ui = Engine::get<UIManager>();

    if (get_current_window_type() == WindowType::VULKAN)
    {
        VulkanWindow &opengl_win = dynamic_cast<VulkanWindow &>(_window);

        _ecs.update_systems(s_delta_time);

        _window.pre_draw();

        _scene_manager.update_current_scene(s_fixed_delta_time);

        _window.on_update();

        _window.post_draw();
    }

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
    auto &_ecs = Engine::get<ECS>();

    _ecs.shutdown_systems();
}

void Application::set_window_title(const std::string &title)
{
    auto &_window = Engine::get<WindowManager>().get_current_window();

    _window.set_window_title(title);
}

void Application::add_scene(const std::string &name, const SceneConfig &config)
{
    auto &_scene_manager = Engine::get<SceneManager>();
    _scene_manager.add_scene(name, std::make_unique<Scene>(config));
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