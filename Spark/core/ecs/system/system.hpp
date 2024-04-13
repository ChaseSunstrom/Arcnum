#ifndef SPARK_SYSTEM_HPP
#define SPARK_SYSTEM_HPP

#include "../../spark.hpp"

#include "../../util/thread_pool.hpp"
#include "../component/component.hpp"
#include "../../util/memory.hpp"

#include "../../net/serializeable.hpp"

namespace spark
{
	class system
	{
	public:
		system() = default;
		virtual ~system() = default;

		virtual void on_init()
		{}

		virtual void on_start()
		{}

		virtual void on_update(f64 delta_time)
		{}

		virtual void on_shutdown()
		{}
		SERIALIZE_EMPTY()
	private:
		component_manager& m_component_manager = component_manager::get();
	};

	class system_manager
	{
	public:
		static system_manager& get()
		{
			static system_manager instance;
			return instance;
		}

		template <typename T, typename... Args>
		T& register_update_system(Args&& ... args)
		{
			std::unique_ptr<T> _system = std::make_unique<T>(m_component_manager, std::forward<Args>(args)...);
			T& s = *_system;
			m_update_systems.push_back(std::static_pointer_cast<system>(std::move(_system)));
			return s;
		}

		template <typename T, typename... Args>
		T& register_start_system(Args&& ... args)
		{
			std::unique_ptr<T> system = std::make_unique<T>(m_component_manager, std::forward<Args>(args)...);
			T& s = *system;
			m_start_systems.push_back(std::move(system));
			return s;
		}

		template <typename T, typename... Args>
		T& register_shutdown_system(Args&& ... args)
		{
			std::unique_ptr<T> system = std::make_unique<T>(m_component_manager, std::forward<Args>(args)...);
			T& s = *system;
			m_shutdown_systems.push_back(std::move(system));
			return s;
		}

		void start_systems()
		{
			for (auto& system : m_start_systems)
			{
				system->on_start();
			}
		}

		void update_systems(f64 delta_time)
		{
			for (auto& system : m_update_systems)
			{
				thread_pool::enqueue(task_priority::HIGH, true,
									 [system = system.get(), delta_time]
									 {
										 system->on_update(delta_time);
									 });
			}
		}

		void shutdown_systems()
		{
			for (auto& system : m_shutdown_systems)
			{
				system->on_shutdown();
			}
		}
	private:
		system_manager() = default;

		~system_manager()
		{
			shutdown_systems();
		}
	private:
		std::vector<std::unique_ptr<system>> m_start_systems = std::vector<std::unique_ptr<system>>();

		std::vector<std::unique_ptr<system>> m_update_systems = std::vector<std::unique_ptr<system>>();

		std::vector<std::unique_ptr<system>> m_shutdown_systems =
			std::vector<std::unique_ptr<system>>();

		component_manager& m_component_manager = component_manager::get();

		SERIALIZE_MEMBERS(
			system_manager,
			m_start_systems,
			m_update_systems,
			m_shutdown_systems
		)
	};
}

#endif