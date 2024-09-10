#ifndef SPARK_SYSTEM_HPP
#define SPARK_SYSTEM_HPP

#include <core/event/event_handler.hpp>
#include <core/pch.hpp>

namespace Spark {
	class System {
	  public:
		System(RefPtr<EventHandler> event_handler)
			: m_event_handler(event_handler) {}
		virtual ~System()                   = default;
		virtual void Start()                = 0;
		virtual void Update(f32 delta_time) = 0;
		virtual void Shutdown()             = 0;

	  protected:
		RefPtr<EventHandler> m_event_handler;
	};

	template<typename T>
	concept IsSystem = std::derived_from<T, System>;
} // namespace Spark

#endif