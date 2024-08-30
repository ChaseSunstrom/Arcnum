#ifndef SPARK_COMPONENT_HPP
#define SPARK_COMPONENT_HPP

#include "entity.hpp"
#include <core/pch.hpp>

namespace Spark {
// Base class for all components
// also stores the id of the entity it's attached to (stored in the Ecs, which
// is why its a friend class)
class Component {
  public:
	friend class Ecs;
	virtual ~Component() = default;
	i64 GetEntityId() const { return m_entity_id; }

  private:
	void SetEntityId(i64 id) { m_entity_id = id; }

  private:
	// The ID of the entity the component is attached to
	i64 m_entity_id;
};
} // namespace Spark

#endif