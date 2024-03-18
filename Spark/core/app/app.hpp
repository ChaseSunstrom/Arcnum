#ifndef SPARK_APP_HPP
#define SPARK_APP_HPP

#include "../events/event.hpp"
#include "../renderer/layer_stack.hpp"
#include "../scene/scene_manager.hpp"
#include "../window/window.hpp"
#include "../audio/audio.hpp"

namespace spark
{
	class application :
			public std::enable_shared_from_this<application>
	{
	public:
		static void on_start();

		static void on_update();

		static bool on_event(std::shared_ptr <event> event);

		static void set_window_title(const std::string& title);

		static void add_scene(const std::string& name, const scene_config& config);

		static void set_delta_time(uint64_t delta_time);

		static uint64_t get_delta_time();

		static renderer& get_renderer();

		static window& get_window();

		static mesh_manager& get_mesh_manager();

		static material_manager& get_material_manager();

		static shader_manager& get_shader_manager();

		static ecs& get_ecs();

		static audio_manager& get_audio_manager();

		static scene& get_scene(const std::string& name);

		static scene& get_current_scene();

	private:
		static uint64_t s_delta_time;

		static std::unique_ptr <window> s_window;

		static std::unique_ptr <renderer> s_renderer;

		static std::unique_ptr <scene_manager> s_scene_manager;

		static std::unique_ptr<mesh_manager> s_mesh_manager;
		
		static std::unique_ptr<material_manager> s_material_manager;
		
		static std::unique_ptr<shader_manager> s_shader_manager;
		
		static std::unique_ptr<audio_manager> s_audio_manager;

		static std::unique_ptr <layer_stack> s_layer_stack;

		static std::unique_ptr<ecs> s_ecs;
	};
}

#endif