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

	template<typename _Ty> class Event : public virtual IEvent {
	  public:
		virtual ~Event() = default;
	};

	template<typename _Ty>
	concept IsEvent                             = std::derived_from<_Ty, Event<_Ty>>;

	template<typename EventType> using EventPtr = SharedPtr<EventType>;

	template<typename... EventTypes> class MultiEvent : public IEvent {
	  public:
		using VariantType = std::variant<SharedPtr<EventTypes>...>;

		MultiEvent(const EventPtr<IEvent>& base_event) { ((TrySetEvent<EventTypes>(base_event)) || ...); }

		template<typename _Ty> bool Is() const { return std::holds_alternative<SharedPtr<_Ty>>(event); }

		template<typename _Ty> const SharedPtr<_Ty>& Get() const { return std::get<SharedPtr<_Ty>>(event); }

		static auto GetEventTypes() { return std::tuple<EventTypes...>{}; }

	  private:
		template<typename _Ty> bool TrySetEvent(const EventPtr<IEvent>& base_event) {
			if (auto derived = DynamicPointerCast<_Ty>(base_event)) {
				event = derived;
				return true;
			}
			return false;
		}

		VariantType event;
	};

	template<typename... EventTypes> using MultiEventPtr = SharedPtr<MultiEvent<EventTypes...>>;

	// ECS Events
	struct EntityCreatedEvent : public Event<EntityCreatedEvent> {
		EntityCreatedEvent(const Entity& entity)
			: entity(entity) {}
		const Entity& entity;
	};

	struct EntityDestroyedEvent : public Event<EntityDestroyedEvent> {
		EntityDestroyedEvent(const Entity& entity)
			: entity(entity) {}
		const Entity& entity;
	};

	enum class ComponentEventType { ADDED, UPDATED, REMOVED };

	class BaseComponentEvent : public virtual IEvent {
	  public:
		BaseComponentEvent(const Entity& entity, ComponentEventType type)
			: entity(entity)
			, type(type) {}

		virtual ~BaseComponentEvent() = default;

		const Entity&      entity;
		ComponentEventType type;
	};

	template<typename _Ty> class ComponentEvent
		: public Event<ComponentEvent<_Ty>>
		, public BaseComponentEvent {
	  public:
		ComponentEvent(const Entity& entity, ComponentEventType type, const _Ty& component)
			: BaseComponentEvent(entity, type)
			, component(component) {}

		const _Ty& component;
	};

	template<typename _Ty> using ComponentAddedEvent   = ComponentEvent<_Ty>;

	template<typename _Ty> using ComponentUpdatedEvent = ComponentEvent<_Ty>;

	template<typename _Ty> using ComponentRemovedEvent = ComponentEvent<_Ty>;

	// Physics Events
	struct CollisionEvent : public Event<CollisionEvent> {
		CollisionEvent(const Entity& entity1, const Entity& entity2)
			: entity1(entity1)
			, entity2(entity2) {}
		const Entity& entity1;
		const Entity& entity2;
	};


	// Input Events
	struct KeyPressedEvent : public Event<KeyPressedEvent> {
		KeyPressedEvent(i32 key, i32 repeat)
			: key(key)
			, repeat(repeat) {}
		i32 key;
		i32 repeat;
	};

	struct KeyReleasedEvent : public Event<KeyReleasedEvent> {
		KeyReleasedEvent(i32 key)
			: key(key) {}
		i32 key;
	};

	struct KeyHeldEvent : public Event<KeyHeldEvent> {
		KeyHeldEvent(i32 key)
			: key(key) {}
		i32 key;
	};

	struct MouseMovedEvent : public Event<MouseMovedEvent> {
		MouseMovedEvent(f64 xpos, f64 ypos)
			: xpos(xpos)
			, ypos(ypos) {}
		f64 xpos;
		f64 ypos;
	};

	struct MouseButtonPressedEvent : public Event<MouseButtonPressedEvent> {
		MouseButtonPressedEvent(i32 button)
			: button(button) {}
		i32 button;
	};

	struct MouseButtonReleasedEvent : public Event<MouseButtonReleasedEvent> {
		MouseButtonReleasedEvent(i32 button)
			: button(button) {}
		i32 button;
	};

	struct MouseScrolledEvent : public Event<MouseScrolledEvent> {
		MouseScrolledEvent(f64 x, f64 y)
			: x(x)
			, y(y) {}
		f64 x;
		f64 y;
	};

	// Window Events
	struct WindowResizedEvent : public Event<WindowResizedEvent> {
		WindowResizedEvent(i32 width, i32 height)
			: width(width)
			, height(height) {}
		i32 width;
		i32 height;
	};

	struct WindowClosedEvent : public Event<WindowClosedEvent> {};

	template <typename _Ty, typename... Args> EventPtr<_Ty> MakeEvent(Args&&... args) {
		return MakeShared<_Ty>(Forward<Args>(args)...);
	}
} // namespace Spark

#endif