#ifndef SPARK_QUERY_HPP
#define SPARK_QUERY_HPP

#include <bitset>
#include <core/pch.hpp>
#include <type_traits>
#include "entity.hpp"
#include "component.hpp"

namespace Spark {
	// Forward declarations
	class Component;
	class EventHandler;
	class Entity;

	// Query filter types
	template<typename T> struct With {
		using FilterType = T;
		static ComponentMask GetMask() {
			ComponentMask mask;
			mask.set(GetComponentId<T>());
			return mask;
		}
	};

	template<typename T> struct Without {
		using FilterType = T;
		static ComponentMask GetMask() {
			ComponentMask mask;
			mask.set(GetComponentId<T>());
			return ~mask;
		}
	};

	template<typename T> struct Changed {
		using FilterType = T;
		static ComponentMask GetChangeMask() {
			ComponentMask mask;
			mask.set(GetComponentId<T>());
			return mask;
		}
	};

	// Parameter wrappers
	template<typename T> class Mut {
	  public:
		explicit Mut(T& value)
			: m_value(value) {}
		T&       Get() { return m_value; }
		const T& Get() const { return m_value; }

	  private:
		T& m_value;
	};

	template<typename T> class Res {
	  public:
		explicit Res(const T& value)
			: m_value(value) {}
		const T& Get() const { return m_value; }

	  private:
		const T& m_value;
	};

	template<typename T> class ResMut {
	  public:
		explicit ResMut(T& value)
			: m_value(value) {}
		T& Get() { return m_value; }
		T& operator*() { return m_value; }
		T* operator->() { return &m_value; }

	  private:
		T& m_value;
	};

	// Query result wrapper
	template<typename... Components> class QueryIterator {
	  public:
		struct Item {
			std::tuple<Components&...> components;
			RefPtr<Entity>             entity;
		};

		QueryIterator(Vector<RefPtr<Entity>>& entities, Vector<UniquePtr<IComponentArray>>& components)
			: m_entities(entities)
			, m_components(components)
			, m_current(0) {
			UpdateCurrent();
		}

		Item operator*() const { return CreateItem(std::make_index_sequence<sizeof...(Components)>{}); }

		QueryIterator& operator++() {
			++m_current;
			UpdateCurrent();
			return *this;
		}

		bool operator!=(const QueryIterator& other) const { return m_current != other.m_current; }

	  private:
		template<size_t... Is> Item CreateItem(std::index_sequence<Is...>) const {
			return Item{std::tie(static_cast<ComponentArray<Components>*>(m_components[Is].Get())->GetData(m_entities[m_current]->GetId())...), m_entities[m_current]};
		}

		void UpdateCurrent() {
			while (m_current < m_entities.Size() && !EntityMatchesQuery(m_entities[m_current])) {
				++m_current;
			}
		}

		bool EntityMatchesQuery(RefPtr<Entity> entity) const { return (entity->template HasComponent<Components>() && ...); }

		Vector<RefPtr<Entity>>&             m_entities;
		Vector<UniquePtr<IComponentArray>>& m_components;
		size_t                              m_current;
	};

	template<typename... Components> class QueryResult {
	  public:
		QueryResult(Vector<RefPtr<Entity>>& entities, Vector<UniquePtr<IComponentArray>>& components)
			: m_entities(entities)
			, m_components(components) {}

		QueryIterator<Components...> begin() { return QueryIterator<Components...>(m_entities, m_components); }

		QueryIterator<Components...> end() { return QueryIterator<Components...>(m_entities, m_components); }

	  private:
		Vector<RefPtr<Entity>>&             m_entities;
		Vector<UniquePtr<IComponentArray>>& m_components;
	};

	// Main Query class
	template<typename... Components> class Query {
	  public:
		Query(Vector<RefPtr<Entity>>& entities, Vector<UniquePtr<IComponentArray>>& components)
			: m_entities(entities)
			, m_components(components) {}

		template<typename Func> void ForEach(Func&& func) {
			QueryResult<Components...> result(m_entities, m_components);
			for (auto item : result) {
				std::apply(Forward<Func>(func), item.components);
			}
		}

		template<typename Func, typename... Filters> void ForEach(Func&& func, Filters... filters) {
			ComponentMask required = (With<Components>::GetMask() | ...);
			ComponentMask excluded = (Without<typename Filters::FilterType>::GetMask() | ...);
			ComponentMask changed  = (Changed<typename Filters::FilterType>::GetChangeMask() | ...);

			QueryResult<Components...> result(m_entities, m_components);
			for (auto item : result) {
				if (EntityMatchesFilters(item.entity, required, excluded, changed)) {
					std::apply(Forward<Func>(func), item.components);
				}
			}
		}

	  private:
		bool EntityMatchesFilters(RefPtr<Entity> entity, const ComponentMask& required, const ComponentMask& excluded, const ComponentMask& changed) const {
			ComponentMask entityMask = entity->GetComponentMask();
			return (entityMask & required) == required && (entityMask & excluded).none() && (changed.none() || (entityMask & changed).any());
		}

		Vector<RefPtr<Entity>>&             m_entities;
		Vector<UniquePtr<IComponentArray>>& m_components;
	};
} // namespace Spark

#endif