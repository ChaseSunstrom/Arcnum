#ifndef SPARK_WINDOW_DATA_HPP
#define SPARK_WINDOW_DATA_HPP

#include "../spark.hpp"
#include "../events/event.hpp"

namespace Spark
{
struct WindowData
{
    WindowData() = default;

    WindowData(std::string title, bool vsync, i32 height, i32 width,
               std::function<void(std::shared_ptr<Event>)> event_callback)
        : m_title(title), m_vsync(vsync), m_height(height), m_width(width), m_event_callback(event_callback)
    {
    }

    virtual ~WindowData() = default;

    std::string m_title = "Title";

    bool m_vsync = false;

    i32 m_width = 1080;

    i32 m_height = 1080;

    std::function<void(std::shared_ptr<Event>)> m_event_callback;
};
}

#endif