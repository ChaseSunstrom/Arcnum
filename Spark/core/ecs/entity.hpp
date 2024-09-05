#ifndef SPARK_ENTITY_HPP
#define SPARK_ENTITY_HPP

#include <core/pch.hpp>
#include "query.hpp"

namespace Spark {

	struct ComponentKey {
		std::type_index type;
		std::string     name;
		bool            operator==(const ComponentKey& other) const = default;
	};

} // namespace Spark

namespace std {
	template<> struct hash<Spark::ComponentKey> {
		size_t operator()(const Spark::ComponentKey& key) const { return (std::hash<std::string>()(key.name) ^ (std::hash<std::type_index>()(key.type) << 1)) >> 1; }
	};
} // namespace std

namespace Spark {

	class Entity {
	  public:
		friend class Ecs;

		operator u32() { return m_id; }
		u32 GetId() const { return m_id; }

		template<IsComponent T> bool HasComponent(const std::string& name) const {
			ComponentKey key = {typeid(T), name};
			return m_components.find(key) != m_components.end();
		}

		template<IsComponent... Ts> bool HasComponents() const { return (HasComponent<Ts>() && ...); }

	  private:
		Entity(u32 id)
			: m_id(id) {}
		void SetId(u32 id) { m_id = id; }

		template<IsComponent T> void AddComponent(const std::string& name) {
			ComponentKey key = {typeid(T), name};
			m_components.insert(key);
		}

		template<IsComponent T> void RemoveComponent(const std::string& name) {
			ComponentKey key = {typeid(T), name};
			m_components.erase(key);
		}

		template<IsComponent T> void RemoveComponents() {
			std::vector<ComponentKey> keys_to_remove;
			for (const auto& key : m_components) {
				if (key.type == typeid(T)) {
					keys_to_remove.push_back(key);
				}
			}
			for (const auto& key : keys_to_remove) {
				m_components.erase(key);
			}
		}

		void RemoveAllComponents() { m_components.clear(); }

	  private:
		u32                              m_id;
		std::unordered_set<ComponentKey> m_components;
	};

} // namespace Spark

#endif