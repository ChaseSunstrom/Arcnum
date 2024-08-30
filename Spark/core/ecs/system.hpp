#ifndef SPARK_SYSTEM_HPP
#define SPARK_SYSTEM_HPP

#include <core/event/event_handler.hpp>
#include <core/pch.hpp>

namespace Spark {
class ISystem {
  public:
	ISystem(EventHandler& event_handler)
		: m_event_handler(event_handler) {}
	virtual ~ISystem()      = default;
	virtual void Start()    = 0;
	virtual void Update()   = 0;
	virtual void Shutdown() = 0;
  protected:
	EventHandler& m_event_handler;
};

template <typename T>
concept IsSystem = std::derived_from<T, ISystem>;
} // namespace Spark

#endif