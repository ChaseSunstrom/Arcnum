#ifndef SPARK_COMPONENT_HPP
#define SPARK_COMPONENT_HPP

#include <core/pch.hpp>
#include <bitset>

namespace Spark {
	class Component {
	  public:
		friend class Ecs;
		virtual ~Component() = default;
		u32 GetEntityId() const { return m_entity_id; }

	  private:
		void SetEntityId(u32 id) { m_entity_id = id; }
		u32  m_entity_id = 0;
	};

	class IComponentArray {
	  public:
		virtual ~IComponentArray()                                     = default;
		virtual void RemoveEntity(u32 entity_id)                       = 0;
		virtual void MoveEntity(u32 entity_id, IComponentArray& other) = 0;
	};

	template<typename _Ty>
	concept IsComponent = DerivedFrom<_Ty, Component>;

	template<IsComponent _Ty> class ComponentArray : public IComponentArray {
	  public:
		void InsertData(const u32 entity_id, const _Ty& component) {
			size_t new_index             = m_data.Size();
			m_entity_to_index[entity_id] = new_index;
			m_index_to_entity[new_index] = entity_id;
			m_data.PushBack(component);
			m_data.Back().SetEntityId(entity_id);
		}

		void RemoveData(const u32 entity_id) {
			if (m_entity_to_index.Find(entity_id) == m_entity_to_index.End()) {
				return;
			}

			size_t removed_index = m_entity_to_index[entity_id];
			size_t last_index    = m_data.Size() - 1;

			if (removed_index != last_index) {
				m_data[removed_index]             = Move(m_data[last_index]);
				u32 last_entity_id                = m_index_to_entity[last_index];
				m_entity_to_index[last_entity_id] = removed_index;
				m_index_to_entity[removed_index]  = last_entity_id;
			}

			m_entity_to_index.Erase(entity_id);
			m_index_to_entity.Erase(last_index);
			m_data.PopBack();
		}

		_Ty& GetData(const u32 entity_id) {
			auto it = m_entity_to_index.Find(entity_id);
			if (it == m_entity_to_index.End()) {
				throw std::runtime_error("Entity does not have this component");
			}
			return m_data[it->second];
		}

		const _Ty& GetData(const u32 entity_id) const {
			auto it = m_entity_to_index.Find(entity_id);
			if (it == m_entity_to_index.End()) {
				throw std::runtime_error("Entity does not have this component");
			}
			return m_data[it->second];
		}

		void RemoveEntity(const u32 entity_id) override { RemoveData(entity_id); }

		void MoveEntity(u32 entity_id, IComponentArray& other) override {
			auto& typed_other = static_cast<ComponentArray<_Ty>&>(other);
			auto  it          = m_entity_to_index.Find(entity_id);
			if (it != m_entity_to_index.End()) {
				typed_other.InsertData(entity_id, GetData(entity_id));
				RemoveEntity(entity_id);
			}
		}

		Vector<_Ty>&       GetAllComponents() { return m_data; }
		const Vector<_Ty>& GetAllComponents() const { return m_data; }

	  private:
		Vector<_Ty>               m_data;
		UnorderedMap<u32, size_t> m_entity_to_index;
		UnorderedMap<size_t, u32> m_index_to_entity;
	};

	constexpr size_t MAX_COMPONENTS = 64;
	using ComponentMask             = std::bitset<MAX_COMPONENTS>;

	inline u32 NextComponentId() {
		static u32 next_id = 0;
		return next_id++;
	}

	// Component ID generator
	template<typename T> inline u32 GetComponentId() {
		static u32 id = NextComponentId();
		return id;
	}
} // namespace Spark

#endif