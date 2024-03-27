#ifndef SPARK_ECS_HPP
#define SPARK_ECS_HPP

#include "../spark.hpp"

#include "component/component_types.hpp"
#include "component/component.hpp"
#include "entity/entity.hpp"
#include "system/system.hpp"
#include "../audio/audio.hpp"
#include "../lighting/light.hpp"

namespace spark
{
	class base_holder
	{
	public:
		virtual ~base_holder() = default;
	};

	template<typename T>
	class holder : public base_holder
	{
	public:
		explicit holder(component_manager& cm)
		{
			m_values = &cm.get_component_array<T>().get_array();
		}

		// Stores a vector of the data
		std::vector<T>* m_values;
	};

	class group
	{
	public:
		group() = default;

		template<typename T>
		void add(component_manager& cm)
		{
			m_map[std::type_index(typeid(T))] = std::make_unique<holder<T>>(cm);
		}

		template<typename T>
		std::vector<T>* get() const
		{
			auto it = m_map.find(std::type_index(typeid(T)));
			if (it != m_map.end())
			{
				// Safely downcast using static_cast since we know the derived type
				holder<T>* _holder = static_cast<holder<T>*>(it->second.get());
				return _holder->m_values; // Use m_values to reference the vector of components
			}
			return nullptr;
		}

	private:
		std::unordered_map<std::type_index, std::unique_ptr<base_holder>> m_map;

	};

	class ecs
	{
	public:

		// ==============================================================================
		// Interface functions for all the managers:
		static ecs& get()
		{
			static ecs instance;
			return instance;
		}

		template <typename T>
		constexpr inline void register_component()
		{
			m_component_manager.register_component<T>();
		}

		template <typename... components>
		constexpr inline void register_components()
		{
			m_component_manager.register_components<components...>();
		}

		template <typename T>
		inline void add_component(entity entity, T component)
		{
			m_component_manager.add_component<T>(entity, component);
		}

		template <typename T>
		inline void set_component(entity entity, T component)
		{
			m_component_manager.set_component<T>(entity, component);
		}

		template <typename T>
		inline T& get_component(entity entity)
		{
			return m_component_manager[entity];
		}

		template <typename T>
		inline component_array<T>& get_component_array()
		{
			return m_component_manager.get_component_array<T>();
		}

		template <typename T>
		inline bool has_component(entity entity)
		{
			return m_component_manager.has_component<T>(entity);
		}

		inline std::vector <component_info_base> get_all_components(entity entity)
		{
			return m_component_manager.get_all_components(entity);
		}

		template <typename T, typename... Args>
		inline T& register_update_system(Args&& ... args)
		{
			return m_system_manager.register_update_system<T>(std::forward<Args>(args)...);
		}

		template <typename T, typename... Args>
		inline T& register_start_system(Args&& ... systems)
		{
			return m_system_manager.register_start_system<T>(std::forward<Args>(systems)...);
		}

		template <typename T, typename... Args>
		inline T& register_shutdown_system(Args&& ... systems)
		{
			return m_system_manager.register_shutdown_system<T>(std::forward<Args>(systems)...);
		}

		inline void start_systems()
		{
			m_system_manager.start_systems();
		}

		inline void update_systems(float64_t delta_time)
		{
			m_system_manager.update_systems(delta_time);
		}

		inline void shutdown_systems()
		{
			m_system_manager.shutdown_systems();
		}

		inline entity create_entity()
		{
			return m_entity_manager.create_entity();
		}

		inline bool entity_is_renderable(entity entity)
		{
			return has_component<mesh_component>(entity) && has_component<transform_component>(entity) &&
				has_component<material_component>(entity);
		}

		inline void destroy_entity(entity entity)
		{
			m_entity_manager.destroy_entity(entity);
			m_component_manager.destroy_component_array(entity);
		}

		template <typename... Components>
		entity create_entity(Components&& ... components)
		{
			entity entity = m_entity_manager.create_entity();

			(add_component(entity, std::forward<Components>(components)), ...);

			// Creates a group based off the components passed in
			std::unique_ptr<group> _group = std::make_unique<group>();
			([&_group, this](auto&& component)
			 {
				 using component_type = std::decay_t<decltype(component)>;
				 _group->add<component_type>(this->m_component_manager);
			 }(std::forward<Components>(components)), ...);  // ... is a fold expression

			// Store the group
			m_groups.emplace_back(std::move(_group));

			return entity;
		}

		// Returns vectors of all components in the group
		template <typename... Components>
		std::tuple<std::vector<Components>*...> query_group_components()
		{
			for (auto& grp : m_groups)
			{
				// This will be true if all components exist in the group for the entity.
				if ((... && (grp->get<Components>() != nullptr)))
				{
					return std::make_tuple(grp->get<Components>()...);
				}
			}

			// If no group with all the requested components is found, return a tuple of nullptrs.
			return std::make_tuple(static_cast<Components*>(nullptr)...);
		}

		// ==============================================================================
	private:
		ecs()
		{
			// Registers engine specific components on initialization of the ECS 
			// custom cumponents will need to be registered manually
			register_components<
				// Rendering
				material_component, transform_component, mesh_component, render_component,
				// Audio
				audio_component,
				// Lighting
				dir_light_component, point_light_component, spot_light_component>();

			register_update_system<audio_system>(&get_component_array<audio_component>().get_array());
		}

		~ecs() = default;

	private:
		std::vector<std::unique_ptr<group>> m_groups = std::vector<std::unique_ptr<group>>();

		component_manager& m_component_manager = component_manager::get();

		entity_manager& m_entity_manager = entity_manager::get();

		system_manager& m_system_manager = system_manager::get();
	};
}

#endif // CORE_ECS_H