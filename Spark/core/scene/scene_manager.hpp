#ifndef SPARK_SCENE_MANAGER_HPP
#define SPARK_SCENE_MANAGER_HPP

#include "../spark.hpp"

#include "scene.hpp"
#include "../renderer/renderer.hpp"

namespace spark
{
	// Seperate file to prevent circular dependencies
	class scene_manager
	{
	public:
		static scene_manager& get()
		{
			static scene_manager instance;
			return instance;
		}

		void add_scene(const std::string& name, std::unique_ptr <scene> scene)
		{
			m_scenes[name] = std::move(scene);
			m_scenes_vector.push_back(&*m_scenes[name]);

			if (m_scenes.size() == 1)
			{
				m_current_scene_name = name;
			}
		}

		void remove_scene(const std::string& name)
		{
			m_scenes.erase(name);
		}

		scene& get_scene(const std::string& name)
		{
			return *m_scenes[name];
		}

		void switch_scene(const std::string& name)
		{
			m_current_scene_name = name;
		}

		scene& get_current_scene()
		{
			return *m_scenes[m_current_scene_name];
		}

		void update_current_scene(f64 time_step)
		{
			scene& current_scene = *m_scenes[m_current_scene_name];
		}

		std::vector <scene*>& get_all_scenes()
		{
			return m_scenes_vector;
		}
	private:
		scene_manager() = default;

		~scene_manager() = default;
	private:
		std::unordered_map <std::string, std::unique_ptr<scene>> m_scenes =
			std::unordered_map <std::string, std::unique_ptr<scene>>();

		std::vector <scene*> m_scenes_vector = std::vector<scene*>();

		std::string m_current_scene_name = "";
	};
}

#endif