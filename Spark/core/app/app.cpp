#include "app.hpp"

#include "app_interface.hpp"
#include "../events/sub.hpp"
#include "../logging/log.hpp"
#include "../ui/ui.hpp"

namespace spark
{
	uint64_t application::s_delta_time = 0.05;
	
	void application::on_start()
	{
		application::add_scene("Main Scene", scene_config(math::vec4(0)));

		auto& _window = engine::get<window>();
		auto& _ecs = engine::get<ecs>();
		auto& _ui = engine::get<ui_manager>();

		_ui.on_start(_window.get_window());

		app_functions::s_on_start();

		_ecs.start_systems();

		subscription<event>::create(EVERY_EVENT_TOPIC, app_functions::s_on_event);

		while (_window.running())
		{
			on_update();
		}
	}

	void application::on_update()
	{
		app_functions::s_on_update();

		auto& _window = engine::get<window>();
		auto& _renderer = engine::get<renderer>();
		auto& _scene_manager = engine::get<scene_manager>();
		auto& _ecs = engine::get<ecs>();
		auto& _ui = engine::get<ui_manager>();

		_ecs.update_systems(s_delta_time);

		_renderer.on_update();

		_window.pre_draw(); 

		_scene_manager.update_current_scene(_renderer.get_fixed_delta_time());

		_window.render_framebuffer_to_screen(); // Renders the framebuffer to the screen

		_ui.on_update(); // UI rendering should come after rendering the framebuffer to screen

		_window.post_draw(); // Swap buffers and poll events


		spark::thread_pool::synchronize_registered_threads();
	}

	void application::on_shutdown()
	{
		auto& _ui = engine::get<ui_manager>();

		_ui.on_shutdown();
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
		auto& _window = engine::get<window>();
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

	uint64_t application::get_delta_time()
	{
		return s_delta_time;
	}
}