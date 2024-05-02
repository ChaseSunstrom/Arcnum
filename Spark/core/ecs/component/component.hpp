#ifndef SPARK_COMPONENT_HPP
#define SPARK_COMPONENT_HPP

#include "../entity/entity_type.hpp"

#include "shader.hpp"
#include "../../net/serializeable.hpp"

#include "../../util/Singleton.hpp"

#include <boost/serialization/access.hpp>

namespace spark
{
	struct IComponentInfo
	{
		IComponentInfo(const std::type_index& type) :
				m_type(type) { }

		virtual ~IComponentInfo() = default;

		virtual const std::type_index& get_type() const
		{
			return m_type;
		}

		std::type_index m_type;
	};

	// For type information when returning all components in an entity
	template <typename T>
	class ComponentInfo :
			public IComponentInfo
	{
	public:
		ComponentInfo(const T& component) :
				IComponentInfo(typeid(T)), m_component(component) { }

		const std::type_index& get_type() const override
		{
			return m_type;
		}

		const T& get_component() const
		{
			return m_component;
		}

	private:
		T m_component;
	};

	struct IComponentArray
	{
		IComponentArray() = default;

		virtual ~IComponentArray() = default;

		virtual void entity_destroyed(Entity entity) = 0;

		virtual bool has_component(Entity entity) const = 0;

		virtual const IComponentInfo get_component(Entity entity) = 0;

		SERIALIZE_EMPTY()
	};

	template <typename T>
	class ComponentArray :
			public IComponentArray
	{
	public:
		ComponentArray() = default;

		std::vector <std::optional<T>>& get_array()
		{
			return m_component_array;
		}

		void insert(Entity entity, T component) 
		{
			if (entity >= m_component_array.size()) 
			{
				m_component_array.resize(entity + 1, std::optional<T>());
			}
			m_component_array[entity] = component;
		}

		void remove(Entity entity)
		{
			if (entity < m_component_array.size())
			{
				m_component_array[entity] = T();
			}
		}

		void entity_destroyed(Entity entity) override
		{
			if (entity < m_component_array.size())
			{
				m_component_array[entity] = T(); // Resets the component to its default state.
			}
		}

		const T& operator[](Entity entity) const
		{
			if (__SPARK_ASSERT__(entity > m_component_array.size()))
			{
				SPARK_ERROR("[ECS] Entity index out of bounds.");

			}

			return m_component_array[entity].value();
		}

		T& operator[](Entity entity)
		{
			if (__SPARK_ASSERT__(entity > m_component_array.size()))
			{
				SPARK_ERROR("[ECS] Entity index out of bounds.");
			}

			return m_component_array[entity].value();
		}

		bool has_component(Entity entity) const override 
		{
			return entity < m_component_array.size() && m_component_array[entity].has_value();
		}

		const IComponentInfo get_component(Entity entity) override 
		{
			return ComponentInfo<T>(m_component_array[entity].value());
		}

		u32 size() const
		{
			return m_component_array.size();
		}

	private:
		std::vector<std::optional<T>> m_component_array;

		SERIALIZE_MEMBERS(ComponentArray, m_component_array)
	};

	class ComponentManager :
		public Singleton<ComponentManager>
	{
	public:
		static ComponentManager& get()
		{
			static ComponentManager instance;
			return instance;
		}

		template <typename T>
		ComponentArray<T>& get_component_array()
		{
			std::string type = std::type_index(typeid(T)).name();

			if (m_components.find(type) == m_components.end())
			{
				// Component type not registered yet, do so now
				register_component<T>();
			}

			return static_cast<ComponentArray<T>&>(*m_components[type]);
		}

		template <typename T>
		constexpr void register_component()
		{
			std::string type = std::type_index(typeid(T)).name();
			m_components[type] = std::make_unique<ComponentArray<T>>();
		}

		template <typename... components>
		constexpr void register_components()
		{
			(register_component<components>(), ...);
		}

		template <typename T>
		void add_component(Entity entity, const T& component)
		{
			get_component_array<T>().insert(entity, component);
		}

		template <typename T>
		void set_component(Entity entity, const T& component)
		{
			get_component_array<T>()[entity] = component;
		}

		template <typename T>
		void remove_component(Entity entity)
		{
			get_component_array<T>().remove(entity);
		}

		template <typename T>
		T& get_component(Entity entity)
		{
			if (__SPARK_ASSERT__(entity > get_component_array<T>().size()))
			{
				SPARK_FATAL("Entity index out of bounds.");
			}

			// Because for some reason this MUST return a const& ????
			return const_cast<T&>(get_component_array<T>()[entity]);
		}

		std::vector <IComponentInfo> get_all_components(Entity entity)
		{
			std::vector <IComponentInfo> components;

			for (auto& [type, array]: m_components)
			{
				if (array->has_component(entity))
				{
					components.push_back(array->get_component(entity));
				}
			}

			return components;
		}

		void destroy_component_array(Entity entity)
		{
			for (const auto& [type, array]: m_components)
			{
				array->entity_destroyed(entity);
			}
		}

		template <typename T>
		bool has_component(Entity id) const 
		{
			std::string type = std::type_index(typeid(T)).name();
			auto found = m_components.find(type);
			
			if (found != m_components.end()) 
			{
				auto array = static_cast<ComponentArray<T>*>(found->second.get());
				return id < array->get_array().size() && array->get_array()[id].has_value(); // Adjusted for optional
			}
			return false;
		}


	private:

		ComponentManager() = default;

		~ComponentManager() = default;

	private:
		std::unordered_map <std::string, std::unique_ptr<IComponentArray>> m_components;

		SERIALIZE_MEMBERS(ComponentManager, m_components)
	};
}

#endif