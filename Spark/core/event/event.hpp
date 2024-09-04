#ifndef SPARK_EVENT_HPP
#define SPARK_EVENT_HPP

#include <core/pch.hpp>

namespace Spark {
class Entity;
class Component;
class BaseEvent {
public:
	virtual ~BaseEvent() = default;
};

template<typename T>
class Event : public virtual BaseEvent {
public:
	virtual ~Event() = default;
};

// ECS Events
struct EntityCreatedEvent : public Event<EntityCreatedEvent> {
    EntityCreatedEvent(const Entity& entity) : entity(entity) {}
    const Entity& entity;
};

struct EntityDestroyedEvent : public Event<EntityDestroyedEvent> {
    EntityDestroyedEvent(const Entity& entity) : entity(entity) {}
    const Entity& entity;
};

enum class ComponentEventType { ADDED, UPDATED, REMOVED };

class BaseComponentEvent : public virtual BaseEvent {
public:
	BaseComponentEvent(const Entity& entity, ComponentEventType type)
		: entity(entity), type(type) {}
    
	virtual ~BaseComponentEvent() = default;
    
	const Entity& entity;
	ComponentEventType type;
};

template<typename T>
class ComponentEvent : public Event<ComponentEvent<T>>, public BaseComponentEvent {
public:
	ComponentEvent(const Entity& entity, ComponentEventType type, const std::shared_ptr<T>& component)
		: BaseComponentEvent(entity, type), component(component) {}

	const std::shared_ptr<T> component;
};

template<typename T>
using ComponentAddedEvent = ComponentEvent<T>;

template<typename T>
using ComponentUpdatedEvent = ComponentEvent<T>;

template<typename T>
using ComponentRemovedEvent = ComponentEvent<T>;

// Input Events
struct KeyPressedEvent : public Event<KeyPressedEvent> {
    KeyPressedEvent(i32 key, i32 repeat) : key(key), repeat(repeat) {}
    i32 key;
    i32 repeat;
};

struct KeyReleasedEvent : public Event<KeyReleasedEvent> {
    KeyReleasedEvent(i32 key) : key(key) {}
    i32 key;
};

struct KeyHeldEvent : public Event<KeyHeldEvent> {
    KeyHeldEvent(i32 key) : key(key) {}
    i32 key;
};

struct MouseMovedEvent : public Event<MouseMovedEvent> {
    MouseMovedEvent(f64 xpos, f64 ypos) : xpos(xpos), ypos(ypos) {}
    f64 xpos;
    f64 ypos;
};

struct MouseButtonPressedEvent : public Event<MouseButtonPressedEvent> {
    MouseButtonPressedEvent(i32 button) : button(button) {}
    i32 button;
};

struct MouseButtonReleasedEvent : public Event<MouseButtonReleasedEvent> {
    MouseButtonReleasedEvent(i32 button) : button(button) {}
    i32 button;
};

struct MouseScrolledEvent : public Event<MouseScrolledEvent> {
    MouseScrolledEvent(f64 x, f64 y) : x(x), y(y) {}
    f64 x;
    f64 y;
};

// Window Events
struct WindowResizedEvent : public Event<WindowResizedEvent> {
    WindowResizedEvent(i32 width, i32 height) : width(width), height(height) {}
    i32 width;
    i32 height;
};

struct WindowClosedEvent : public Event<WindowClosedEvent> {};
} // namespace Spark

#endif