#ifndef SPARK_ENTITY_HPP
#define SPARK_ENTITY_HPP

#include <core/pch.hpp>
#include "component.hpp"

namespace Spark {
	class Entity {
	  public:
		friend class Ecs;

		operator u32() { return m_id; }
		u32 GetId() const { return m_id; }

		const ComponentMask& GetComponentMask() const { return m_component_mask; }

		template<IsComponent T> bool HasComponent() const { return m_component_mask.test(GetComponentId<T>()); }

		template<IsComponent... Ts> bool HasComponents() const { return (HasComponent<Ts>() && ...); }

	  private:
		Entity(u32 id)
			: m_id(id) {}

		template<IsComponent T> void AddComponent() { m_component_mask.set(GetComponentId<T>()); }

		template<IsComponent T> void RemoveComponent() { m_component_mask.reset(GetComponentId<T>()); }

		void RemoveAllComponents() { m_component_mask.reset(); }

		u32           m_id;
		ComponentMask m_component_mask;
	};
} // namespace Spark

#endif
