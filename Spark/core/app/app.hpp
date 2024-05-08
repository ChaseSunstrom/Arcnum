#ifndef SPARK_APP_HPP
#define SPARK_APP_HPP

#include "../audio/audio.hpp"
#include "../events/event.hpp"
#include "../renderer/layer_stack.hpp"
#include "../scene/scene_manager.hpp"
#include "../util/singleton.hpp"
#include "../window/window.hpp"

#include "../util/timer.hpp"

namespace Spark {
class Application : public Singleton<Application> {
public:
  static void on_start();

  static void on_update();

  static void on_event(std::shared_ptr<Event> event);

  static void on_shutdown();

  static void set_window_title(const std::string &title);

  static void add_scene(const std::string &name, const SceneConfig &config);

  static void set_delta_time(u64 delta_time);

  static void calculate_total_time();

  static void calculate_delta_time();

  static void calculate_last_frame_time();

  static f64 get_fixed_delta_time();

  static f64 get_delta_time();

  static f64 get_last_frame_time();

  static f64 get_total_time();

  static u64 get_tick_speed();

  static void set_fixed_delta_time(f64 time);

  static void set_tick_speed(u64 speed);

  static Scene &get_current_scene();

private:
  static f64 s_fixed_delta_time;

  static f64 s_delta_time;

  static f64 s_last_frame_time;

  static f64 s_total_time;

  static u64 s_tick_speed;

  static std::unique_ptr<Timer> s_timer;
};
} // namespace spark

#endif