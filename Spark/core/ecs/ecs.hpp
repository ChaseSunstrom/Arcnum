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
	class ecs
	{
	public:

		// ==============================================================================
		// Interface functions for all the managers:

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

		template <typename T>
		constexpr inline void register_component()
		{
			m_component_manager->register_component<T>();
		}

		template <typename... components>
		constexpr inline void register_components()
		{
			m_component_manager->register_components<components...>();
		}

		template <typename T>
		inline void add_component(entity entity, T component)
		{
			m_component_manager->add_component<T>(entity, component);
		}

		template <typename T>
		inline void set_component(entity entity, T component)
		{
			m_component_manager->set_component<T>(entity, component);
		}

		template <typename T>
		inline T& get_component(entity entity)
		{
			return m_component_manager[entity];
		}

		template <typename T>
		inline component_array<T>& get_component_array()
		{
			return m_component_manager->get_component_array<T>();
		}

		template <typename T>
		inline bool has_component(entity entity)
		{
			return m_component_manager->has_component<T>(entity);
		}

		inline std::vector <component_info_base> get_all_components(entity entity)
		{
			return m_component_manager->get_all_components(entity);
		}

		template <typename T, typename... Args>
		inline T& register_update_system(Args&& ... args)
		{
			return m_system_manager->register_update_system<T>(std::forward<Args>(args)...);
		}

		template <typename T, typename... Args>
		inline T& register_start_system(Args&& ... systems)
		{
			return m_system_manager->register_start_system<T>(std::forward<Args>(systems)...);
		}

		template <typename T, typename... Args>
		inline T& register_shutdown_system(Args&& ... systems)
		{
			return m_system_manager->register_shutdown_system<T>(std::forward<Args>(systems)...);
		}

		inline void start_systems()
		{
			m_system_manager->start_systems();
		}

		inline void update_systems(float64_t delta_time)
		{
			m_system_manager->update_systems(delta_time);
		}

		inline void shutdown_systems()
		{
			m_system_manager->shutdown_systems();
		}

		inline entity create_entity()
		{
			return m_entity_manager->create_entity();
		}

		inline bool entity_is_renderable(entity entity)
		{
			return has_component<mesh_component>(entity) && has_component<transform_component>(entity) &&
				has_component<material_component>(entity);
		}

		inline void destroy_entity(entity entity)
		{
			m_entity_manager->destroy_entity(entity);
			m_component_manager->destroy_component_array(entity);
		}

		template <typename... Components>
		entity create_entity(Components&& ... components)
		{
			entity entity = m_entity_manager->create_entity();

			(add_component(entity, std::forward<Components>(components)), ...);

			return entity;
		}

		// ==============================================================================
		
	private:
		std::unique_ptr <component_manager> m_component_manager = std::make_unique<component_manager>();

		std::unique_ptr <entity_manager> m_entity_manager = std::make_unique<entity_manager>();

		std::unique_ptr <system_manager> m_system_manager = std::make_unique<system_manager>(*m_component_manager);
	};
}

#endif // CORE_ECS_H