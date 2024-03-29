#ifndef SPARK_ECS_HPP
#define SPARK_ECS_HPP

#include "../spark.hpp"
#include "../audio/audio.hpp"
#include "../lighting/light.hpp"
#include "observer.hpp"
#include "component/component.hpp"
#include "component/component_types.hpp"
#include "entity/entity.hpp"
#include "system/system.hpp"

namespace spark
{
	struct entity_created_event : public event
	{
		entity_created_event(entity entity, const std::unordered_map<std::type_index, component_info_base>& components)
			: event(ENTITY_CREATED_EVENT), m_entity(entity), m_components(components)
		{ }

		template <typename T>
		T get_component()
		{
			return static_cast<component_info<T>>(m_components[typeid(T)]).m_type;
		}

		entity m_entity;
		std::unordered_map<std::type_index, component_info_base> m_components;
	};

	struct entity_updated_event : public event
	{
		entity_updated_event(entity entity, const std::unordered_map<std::type_index, component_info_base>& components)
			: event(ENTITY_UPDATED_EVENT), m_entity(entity), m_components(components) { }

		template <typename T>
		T get_component()
		{
			return static_cast<component_info<T>>(m_components[typeid(T)]).m_type;
		}

		entity m_entity;
		std::unordered_map<std::type_index, component_info_base> m_components;
	};

	struct entity_destroyed_event : public event
	{
		entity_destroyed_event(entity entity) 
			: event(ENTITY_DESTROYED_EVENT), m_entity(entity) {}

		entity m_entity;
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

			std::shared_ptr<entity_destroyed_event> event = std::make_shared<entity_destroyed_event>(entity);
		}

		void add_observer(observer* observer)
		{
			m_observers.push_back(std::move(observer));
		}

		void remove_observer(observer& _observer)
		{
			m_observers.erase(std::remove_if(m_observers.begin(), m_observers.end(),
				[&_observer](const std::unique_ptr<observer>& o)
				{
					return o.get() == &_observer;
				}),
				m_observers.end());
		}

		template <typename... Components>
		entity create_entity(Components&& ... components)
		{
			entity entity = m_entity_manager.create_entity();

			(add_component(entity, std::forward<Components>(components)), ...);

			std::unordered_map<std::type_index, component_info_base> components_map;

			(components_map[typeid(Components)] = component_info<Components>(), ...);

			std::shared_ptr<entity_created_event> event = std::make_shared<entity_created_event>(entity, components_map);

			notify_observers(event);

			return entity;
		}

		template<typename Pred>
		void remove_if(Pred predicate)
		{
			for (auto it = m_entity_manager.begin(); it != m_entity_manager.end(); )
			{
				if (predicate(*it))
				{
					destroy_entity(*it);
					it = m_entity_manager.erase(it);
				}
				else
				{
					++it;
				}
			}
		}

		template<typename Pred, typename UpdateFunc>
		void update_if(Pred predicate, UpdateFunc update_func)
		{
			for (auto& entity : m_entity_manager.get_all_entities())
			{
				if (predicate(entity))
				{
					// Assuming we want to update a specific component type for the matched entities
					if (has_component<SomeComponent>(entity))
					{
						auto& comp = get_component<SomeComponent>(entity);
						update_func(comp);
					}
				}
			}
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

		void notify_observers(std::shared_ptr<event> event)
		{
			thread_pool::enqueue(task_priority::VERY_HIGH, false, [this, event]()
				{
					for (auto& observer : m_observers)
					{
						observer->on_notify(event);
					}
				});
		}

	private:
		std::vector<observer*> m_observers;

		component_manager& m_component_manager = component_manager::get();

		entity_manager& m_entity_manager = entity_manager::get();

		system_manager& m_system_manager = system_manager::get();
	};
}

#endif // CORE_ECS_H