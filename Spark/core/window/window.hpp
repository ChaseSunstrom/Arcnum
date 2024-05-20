#ifndef SPARK_WINDOW_H
#define SPARK_WINDOW_H

#include "window_data.hpp"
#include "../events/event.hpp"
#include "../logging/log.hpp"
#include "../spark.hpp"

namespace Spark
{

class Window
{
  public:
    bool initialized()
    {
        return m_initialized;
    }


    virtual void pre_draw() = 0;

    virtual void on_update() = 0;

    virtual void draw() = 0;

    virtual void post_draw() = 0;

    virtual bool is_running() const = 0;

    virtual void set_vsync(bool vsync) = 0;

    virtual void set_window_title(const std::string &title) = 0;
    
    virtual void init() = 0;

    virtual WindowData &get_window_data() const = 0;

  protected:
    Window() = default;

    virtual ~Window() = default;
  protected:
    bool m_running = true;
    
    bool m_initialized = false;
};

} // namespace Spark

#endif // SPARK_WINDOW_HPP