#include "app.hpp"

#include "app_interface.hpp"
#include "../events/sub.hpp"
#include "../logging/log.hpp"
#include "../ui/ui.hpp"
#include "../window/window_manager.hpp"

namespace spark
{
	float64_t application::s_fixed_delta_time = 0.005;
	float64_t application::s_delta_time = 0;
	float64_t application::s_last_frame_time = 0;
	float64_t application::s_total_time = 0;
	uint64_t application::s_tick_speed = 60;
	std::unique_ptr<timer> application::s_timer = std::make_unique<timer>();

	void application::on_start()
	{
		application::add_scene("Main Scene", scene_config(math::vec4(0)));

		auto& _window_man = window_manager::get();
		auto& _window = _window_man.get_current_window();
		auto& _ecs = engine::get<ecs>();
		auto& _ui = engine::get<ui_manager>();

		app_functions::s_on_start();

		_ecs.start_systems();

		subscription<event>::create(EVERY_EVENT_TOPIC, app_functions::s_on_event);

		s_timer->start();

		while (_window.is_running())
		{
			on_update();
		}
	}

	void application::on_update()
	{
		calculate_delta_time();
		SPARK_INFO("[APPLICATION] Delta time: " << s_last_frame_time);

		app_functions::s_on_update();

		auto& _window = engine::get<window_manager>().get_current_window();
		auto& _renderer = engine::get<renderer>();
		auto& _scene_manager = engine::get<scene_manager>();
		auto& _ecs = engine::get<ecs>();
		auto& _ui = engine::get<ui_manager>();

		if (get_current_window_type() == window_type::VULKAN)
		{
			vulkan_window& opengl_win = dynamic_cast<vulkan_window&>(_window);

			_ecs.update_systems(s_delta_time);

			_window.pre_draw();

			//_scene_manager.update_current_scene(_renderer.get_fixed_delta_time());

			_window.on_update();

			//_ui.on_update();

			_window.post_draw();
		}

		spark::thread_pool::synchronize_registered_threads();
	}

	void application::on_event(std::shared_ptr <event> event)
	{
		thread_pool::enqueue(task_priority::CRITICAL, false, [event]()
			{
				spark::event_dispatcher dispatcher(event);
				return dispatcher.dispatch(app_functions::s_on_event);
			});
	}

	void application::set_window_title(const std::string& title)
	{
		auto& _window = engine::get<window_manager>().get_current_window();

		_window.set_window_title(title);
	}

	void application::add_scene(const std::string& name, const scene_config& config)
	{
		auto& _scene_manager = engine::get<scene_manager>();
		_scene_manager.add_scene(name, std::make_unique<scene>(config));
	}

	void application::set_delta_time(uint64_t delta_time)
	{
		s_delta_time = delta_time;
	}

	void application::calculate_delta_time()
	{
		// Update last frame time for frame rate calculations
		float64_t time = s_timer->elapsed_milliseconds();
		s_last_frame_time = time - s_total_time;
		s_delta_time = s_last_frame_time;
		s_total_time = time;
	}

	float64_t application::get_delta_time()
	{
		return s_last_frame_time / 1000.0; // Convert milliseconds to seconds
	}

	float64_t application::get_fixed_delta_time()
	{
		return s_fixed_delta_time;
	}

	float64_t application::get_last_frame_time()
	{
		return s_last_frame_time;
	}

	float64_t application::get_total_time()
	{
		return s_total_time;
	}

	uint64_t application::get_tick_speed()
	{
		return s_tick_speed;
	}

	void application::set_fixed_delta_time(float64_t time)
	{
		s_fixed_delta_time = time;
	}

	void application::set_tick_speed(uint64_t speed)
	{
		s_tick_speed = speed;
	}
}