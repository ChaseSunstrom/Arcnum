#include "app.hpp"

#include "app_interface.hpp"
#include "../events/sub.hpp"
#include "../logging/log.hpp"

namespace spark
{
	uint64_t application::s_delta_time = 0.05;

	std::unique_ptr<window> application::s_window = std::make_unique<window>();

	std::unique_ptr<renderer> application::s_renderer = std::make_unique<renderer>();

	std::unique_ptr<ecs> application::s_ecs = std::make_unique<ecs>();

	std::unique_ptr<scene_manager> application::s_scene_manager = std::make_unique<scene_manager>(application::get_renderer());

	std::unique_ptr<layer_stack> application::s_layer_stack = std::make_unique<layer_stack>();

	std::unique_ptr<mesh_manager> application::s_mesh_manager = std::make_unique<mesh_manager>();

	std::unique_ptr<material_manager> application::s_material_manager = std::make_unique<material_manager>();

	std::unique_ptr<shader_manager> application::s_shader_manager = std::make_unique<shader_manager>();

	std::unique_ptr<audio_manager> application::s_audio_manager = std::make_unique<audio_manager>();

	std::unique_ptr<texture_manager> application::s_texture_manager = std::make_unique<texture_manager>();

	void application::on_start()
	{
		application::add_scene("Main Scene", scene_config(math::vec4(0)));

		app_functions::s_on_start();

		s_ecs->start_systems();

		subscription<event>::create(EVERY_EVENT_TOPIC, app_functions::s_on_event);

		while (s_window->running())
		{
			on_update();
		}
	}

	void application::on_update()
	{
		app_functions::s_on_update();
		
		s_ecs->update_systems(s_delta_time);

		s_renderer->on_update();

		s_scene_manager->update_current_scene(s_renderer->get_fixed_delta_time());

		s_window->on_update();

		spark::thread_pool::synchronize_registered_threads();
	}

	bool application::on_event(std::shared_ptr <event> event)
	{
		spark::event_dispatcher dispatcher(event);

		return dispatcher.dispatch(app_functions::s_on_event);
	}

	void application::set_window_title(const std::string& title)
	{
		s_window->set_window_title(title);
	}

	void application::add_scene(const std::string& name, const scene_config& config)
	{
		s_scene_manager->add_scene(name, std::make_unique<scene>(config));
	}

	void application::set_delta_time(uint64_t delta_time)
	{
		s_delta_time = delta_time;
	}

	uint64_t application::get_delta_time()
	{
		return s_delta_time;
	}

	scene& application::get_current_scene()
	{
		return s_scene_manager->get_current_scene();
	}

	window& application::get_window()
	{
		return *s_window;
	}

	mesh_manager& application::get_mesh_manager()
	{
		return *s_mesh_manager;
	}

	texture_manager& application::get_texture_manager()
	{
		return *s_texture_manager;
	}

	material_manager& application::get_material_manager()
	{
		return *s_material_manager;
	}

	shader_manager& application::get_shader_manager()
	{
		return *s_shader_manager;
	}

	audio_manager& application::get_audio_manager()
	{
		return *s_audio_manager;
	}

	scene& application::get_scene(const std::string& name)
	{
		return s_scene_manager->get_scene(name);
	}

	ecs& application::get_ecs()
	{
		return *s_ecs;
	}

	renderer& application::get_renderer()
	{
		return *s_renderer;
	}
}