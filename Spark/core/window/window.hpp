#ifndef SPARK_WINDOW_H
#define SPARK_WINDOW_H

#include "../events/event.hpp"
#include "../logging/log.hpp"
#include "../spark.hpp"

#include "window_type.hpp"

namespace Spark {
struct WindowData {
  WindowData() = default;

  WindowData(std::string title, bool vsync, i32 height, i32 width,
             std::function<void(std::shared_ptr<Event>)> event_callback)
      : m_title(title), m_vsync(vsync), m_height(height), m_width(width),
        m_event_callback(event_callback) {}

  virtual ~WindowData() = default;

  std::string m_title = "Title";

  bool m_vsync = false;

  i32 m_width = 1080;

  i32 m_height = 1080;

  std::function<void(std::shared_ptr<Event>)> m_event_callback;
};

class Window {
public:
  virtual void pre_draw() = 0;

  virtual void on_update() = 0;

  virtual void post_draw() = 0;

  virtual bool is_running() = 0;

  virtual void set_vsync(bool vsync) = 0;

  virtual void set_window_title(const std::string &title) = 0;

  virtual WindowData &get_window_data() const = 0;

  WindowType get_window_type() const { return m_type; }

protected:
  Window(WindowType type) : m_type(type) {}

  virtual ~Window() = default;

  bool m_running = true;

  WindowType m_type = WindowType::UNKNOWN;
};

} // namespace spark

#endif // CORE_WINDOW_H