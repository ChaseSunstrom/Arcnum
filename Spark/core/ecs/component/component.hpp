#ifndef SPARK_COMPONENT_HPP
#define SPARK_COMPONENT_HPP

#include "../entity/entity_type.hpp"

#include "shader.hpp"

namespace spark
{
	struct component_info_base
	{
		component_info_base(const std::type_index& type) :
			m_type(type)
		{}

		virtual ~component_info_base() = default;

		virtual const std::type_index& get_type() const
		{
			return m_type;
		}

		std::type_index m_type;
	};

	// For type information when returning all components in an entity
	template <typename T>
	class component_info :
		public component_info_base
	{
	public:
		component_info(const T& component) :
			component_info_base(typeid(T)), m_component(component)
		{}

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

	struct component_array_base
	{
		virtual ~component_array_base() = default;

		virtual void entity_destroyed(entity entity) = 0;

		virtual bool has_component(entity entity) const = 0;

		virtual const component_info_base get_component(entity entity) = 0;
	};

	template <typename T>
	class component_array : public component_array_base
	{
	public:
		std::vector <T>& get_array()
		{
			return m_component_array;
		}

		void insert(entity entity, T component)
		{
			if (entity >= m_component_array.size())
			{
				m_component_array.resize(entity + 1);
			}

			m_component_array[entity] = component;
		}

		void remove(entity entity)
		{
			if (entity < m_component_array.size())
			{
				m_component_array[entity] = T();
			}
		}

		void entity_destroyed(entity entity)
		{
			m_component_array[entity] = T();
		}

		const T& operator[](entity entity) const
		{
			if (__SPARK_ASSERT__(entity > m_component_array.size()))
			{
				SPARK_FATAL("Entity index out of bounds.");
			}

			return m_component_array[entity];
		}

		bool has_component(entity entity) const override
		{
			return entity < m_component_array.size();
		}

		const component_info_base get_component(entity entity) override
		{
			return component_info<T>(m_component_array[entity]);
		}

	private:
		std::vector <T> m_component_array = std::vector<T>();
	};

	class component_manager
	{
	public:
		static component_manager& get()
		{
			static component_manager instance;
			return instance;
		}
		template <typename T>
		component_array<T>& get_component_array()
		{
			std::type_index type = std::type_index(typeid(T));

			if (m_components.find(type) == m_components.end())
			{
				// Component type not registered yet, do so now
				register_component<T>();
			}

			return static_cast<component_array<T>&>(*m_components[type]);
		}

		template <typename T>
		constexpr  void register_component()
		{
			std::type_index type = std::type_index(typeid(T));
			m_components[type] = std::make_unique<component_array<T>>();
		}

		template <typename... components>
		constexpr  void register_components()
		{
			(register_component<components>(), ...);
		}

		template <typename T>
		void add_component(entity entity, T component)
		{
			get_component_array<T>().insert(entity, component);
		}

		template <typename T>
		void set_component(entity entity, T component)
		{
			get_component_array<T>()[entity] = component;
		}

		template <typename T>
		void remove_component(entity entity)
		{
			get_component_array<T>().remove(entity);
		}

		template <typename T>
		T& operator[](entity entity)
		{
			if (__SPARK_ASSERT__(entity > get_component_array<T>().size()))
			{
				SPARK_FATAL("Entity index out of bounds.");
			}

			return get_component_array<T>()[entity];
		}

		std::vector <component_info_base> get_all_components(entity entity)
		{
			std::vector <component_info_base> components;

			for (auto& [type, array] : m_components)
			{
				if (array->has_component(entity))
				{
					components.push_back(array->get_component(entity));
				}
			}

			return components;
		}

		void destroy_component_array(entity entity)
		{
			for (const auto& [type, array] : m_components)
			{
				array->entity_destroyed(entity);
			}
		}

		template <typename T>
		bool has_component(entity id) const
		{
			std::type_index type = std::type_index(typeid(T));

			auto found = m_components.find(type);

			if (found != m_components.end())
			{
				auto array = static_cast<component_array<T>*>(found->second.get());
				return id < array->get_array().size() && array->get_array()[id] != T();
			}

			return false;
		}
	private:

		component_manager() = default;

		~component_manager() = default;

	private:
		std::unordered_map <std::type_index, std::unique_ptr<component_array_base>> m_components;
	};
}

#endif