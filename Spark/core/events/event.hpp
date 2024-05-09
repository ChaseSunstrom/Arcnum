#ifndef SPARK_EVENT_HPP
#define SPARK_EVENT_HPP

#include "../spark.hpp"

namespace Spark
{

struct Event
{
    Event() = default;

    Event(i32 type) : m_type(type)
    {
    }

    virtual ~Event() = default;

    bool m_handled = false;

    i32 m_type = NO_EVENT;
};

using WindowClosedEvent = Event;

struct WindowResizedEvent : Event
{
    WindowResizedEvent() = default;

    WindowResizedEvent(i32 width, i32 height) : Event(WINDOW_RESIZED_EVENT), m_width(width), m_height(height)
    {
    }

    i32 m_width;

    i32 m_height;
};

struct WindowMovedEvent : Event
{
    WindowMovedEvent() = default;

    WindowMovedEvent(i32 x_pos, i32 y_pos) : Event(WINDOW_MOVED_EVENT), m_x_pos(x_pos), m_y_pos(y_pos)
    {
    }

    i32 m_x_pos;

    i32 m_y_pos;
};

struct KeyPressedEvent : Event
{
    KeyPressedEvent() = default;

    KeyPressedEvent(i32 key_code) : Event(KEY_PRESSED_EVENT), m_key_code(key_code)
    {
    }

    i32 m_key_code;
};

struct KeyReleasedEvent : Event
{
    KeyReleasedEvent(i32 key_code) : Event(KEY_RELEASED_EVENT), m_key_code(key_code)
    {
    }

    i32 m_key_code;
};

struct KeyRepeatedEvent : Event
{
    KeyRepeatedEvent() = default;

    KeyRepeatedEvent(i32 key_code) : Event(KEY_REPEAT_EVENT), m_key_code(key_code)
    {
    }

    i32 m_key_code;
};

struct MousePressedEvent : Event
{
    MousePressedEvent() = default;

    MousePressedEvent(i32 button) : Event(MOUSE_PRESSED_EVENT), m_button(button)
    {
    }

    i32 m_button;
};

struct MouseReleasedEvent : Event
{
    MouseReleasedEvent() = default;

    MouseReleasedEvent(i32 button) : Event(MOUSE_RELEASED_EVENT), m_button(button)
    {
    }

    i32 m_button;
};

struct MouseMovedEvent : Event
{
    MouseMovedEvent() = default;

    MouseMovedEvent(f64 x_pos, f64 y_pos) : Event(MOUSE_MOVE_EVENT), m_x_pos(x_pos), m_y_pos(y_pos)
    {
    }

    f64 m_x_pos;

    f64 m_y_pos;
};

struct MouseScrolledEvent : Event
{
    MouseScrolledEvent() = default;

    MouseScrolledEvent(f64 x_offset, f64 y_offset)
        : Event(MOUSE_SCROLLED_EVENT), m_x_offset(x_offset), m_y_offset(y_offset)
    {
    }

    f64 m_x_offset;

    f64 m_y_offset;
};

// ===============================================================

class EventDispatcher
{
  public:
    EventDispatcher() = default;

    EventDispatcher(std::shared_ptr<Event> event) : m_event(event)
    {
    }

    inline bool dispatch(std::function<bool(std::shared_ptr<Event>)> fn)
    {
        return fn(m_event);
    }

  private:
    std::shared_ptr<Event> m_event;
};
} // namespace Spark

#endif // CORE_EVENT_H