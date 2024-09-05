#ifndef SPARK_EVENT_HPP
#define SPARK_EVENT_HPP

#include <core/pch.hpp>

namespace Spark {
class Entity;
class Component;
class IEvent {
public:
	virtual ~IEvent() = default;
};

template<typename T>
class Event : public virtual IEvent {
public:
	virtual ~Event() = default;
};

template <typename T>
concept IsEvent = std::derived_from<T, Event<T>>;

template <typename EventType>
using EventPtr  = std::shared_ptr<EventType>;

template <typename... EventTypes>
class MultiEvent : public IEvent {
  public:
	using VariantType = std::variant<std::shared_ptr<EventTypes>...>;

	MultiEvent(const EventPtr<IEvent>& base_event) {
		((TrySetEvent<EventTypes>(base_event)) || ...);
	}

	template <typename T>
	bool Is() const {
		return std::holds_alternative<std::shared_ptr<T>>(event);
	}

	template <typename T>
	const std::shared_ptr<T>& Get() const {
		return std::get<std::shared_ptr<T>>(event);
	}

	static auto GetEventTypes() {
		return std::tuple<EventTypes...>{};
	}

  private:
	template <typename T>
	bool TrySetEvent(const EventPtr<IEvent>& base_event) {
		if (auto derived = std::dynamic_pointer_cast<T>(base_event)) {
			event = derived;
			return true;
		}
		return false;
	}

	VariantType event;
};

template<typename... EventTypes>
using MultiEventPtr = std::shared_ptr<MultiEvent<EventTypes...>>;

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

class BaseComponentEvent : public virtual IEvent {
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
	ComponentEvent(const Entity& entity, ComponentEventType type, const T& component)
		: BaseComponentEvent(entity, type), component(component) {}

	const T& component;
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