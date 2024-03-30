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

		static void on_event(std::shared_ptr <event> event);

		static void on_shutdown();

		static void set_window_title(const std::string& title);

		static void add_scene(const std::string& name, const scene_config& config);

		static void set_delta_time(uint64_t delta_time);

		static uint64_t get_delta_time();
	private:
		static uint64_t s_delta_time;
	};
}

#endif