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

template <IsComponent T>
class ComponentArray : public IComponentArray {
  public:
	void InsertData(const u32 entity_id, const T& component) {
		size_t new_index             = m_data.size();
		m_entity_to_index[entity_id] = new_index;
		m_index_to_entity[new_index] = entity_id;
		m_data.push_back(component);
	}

	void RemoveData(const u32 entity_id) {
		size_t removed_index              = m_entity_to_index[entity_id];
		size_t last_index                 = m_data.size() - 1;
		m_data[removed_index]             = std::move(m_data[last_index]);

		u32 last_entity_id                = m_index_to_entity[last_index];
		m_entity_to_index[last_entity_id] = removed_index;
		m_index_to_entity[removed_index]  = last_entity_id;

		m_entity_to_index.erase(entity_id);
		m_index_to_entity.erase(last_index);

		m_data.pop_back();
	}

	T& GetData(const u32 entity_id) {
		return m_data[m_entity_to_index[entity_id]];
	}

	void RemoveEntity(const u32 entity_id) override {
		if (m_entity_to_index.find(entity_id) != m_entity_to_index.end()) {
			RemoveData(entity_id);
		}
	}

	Query<T>& GetAllComponents() {
		return m_data;
	}

  private:
	Query<T> m_data;
	std::unordered_map<u32, size_t> m_entity_to_index;
	std::unordered_map<size_t, u32> m_index_to_entity;
};

} // namespace Spark

#endif