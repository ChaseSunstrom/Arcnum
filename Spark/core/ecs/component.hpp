#ifndef SPARK_COMPONENT_HPP
#define SPARK_COMPONENT_HPP

#include <core/pch.hpp>
#include "entity.hpp"

namespace Spark {
	// Base class for all components
	// also stores the id of the entity it's attached to (stored in the Ecs, which
	// is why its a friend class)
	class Component {
	  public:
		friend class Ecs;
		virtual ~Component() = default;
		u32 GetEntityId() const { return m_entity_id; }

	  private:
		void SetEntityId(u32 id) { m_entity_id = id; }

	  private:
		// The ID of the entity the component is attached to
		u32 m_entity_id;
	};

	class IComponentArray {
	  public:
		virtual ~IComponentArray()               = default;
		virtual void RemoveEntity(u32 entity_id) = 0;
	};

	template<IsComponent _Ty> class ComponentArray : public IComponentArray {
	  public:
		void InsertData(const u32 entity_id, const _Ty& component) {
			size_t new_index             = m_data.Size();
			m_entity_to_index[entity_id] = new_index;
			m_index_to_entity[new_index] = entity_id;
			m_data.PushBack(component);
		}

		void RemoveData(const u32 entity_id) {
			size_t removed_index              = m_entity_to_index[entity_id];
			size_t last_index                 = m_data.Size() - 1;
			m_data[removed_index]             = Move(m_data[last_index]);

			u32 last_entity_id                = m_index_to_entity[last_index];
			m_entity_to_index[last_entity_id] = removed_index;
			m_index_to_entity[removed_index]  = last_entity_id;

			m_entity_to_index.Erase(entity_id);
			m_index_to_entity.Erase(last_index);

			m_data.PopBack();
		}

		_Ty& GetData(const u32 entity_id) { return m_data[m_entity_to_index[entity_id]]; }

		void RemoveEntity(const u32 entity_id) override {
			if (m_entity_to_index.find(entity_id) != m_entity_to_index.end()) {
				RemoveData(entity_id);
			}
		}

		Query<_Ty>& GetAllComponents() { return m_data; }

	  private:
		Query<_Ty>                        m_data;
		UnorderedMap<u32, size_t> m_entity_to_index;
		UnorderedMap<size_t, u32> m_index_to_entity;
	};

} // namespace Spark

#endif