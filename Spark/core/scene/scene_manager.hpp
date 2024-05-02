#ifndef SPARK_SCENE_MANAGER_HPP
#define SPARK_SCENE_MANAGER_HPP

#include "../spark.hpp"

#include "scene.hpp"
#include "../renderer/renderer.hpp"

namespace spark
{
	// Seperate file to prevent circular dependencies
	class SceneManager
	{
	public:
		static SceneManager& get()
		{
			static SceneManager instance;
			return instance;
		}

		void add_scene(const std::string& name, std::unique_ptr<Scene> scene)
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

		Scene& get_scene(const std::string& name)
		{
			return *m_scenes[name];
		}

		void switch_scene(const std::string& name)
		{
			m_current_scene_name = name;
		}

		Scene& get_current_scene()
		{
			return *m_scenes[m_current_scene_name];
		}

		void update_current_scene(f64 time_step)
		{
			Scene& current_scene = *m_scenes[m_current_scene_name];
		}

		std::vector<Scene*>& get_all_scenes()
		{
			return m_scenes_vector;
		}

	private:
		SceneManager() = default;

		~SceneManager() = default;

	private:
		std::unordered_map<std::string, std::unique_ptr<Scene>> m_scenes = std::unordered_map<std::string, std::unique_ptr<Scene>>();

		std::vector<Scene*> m_scenes_vector = std::vector<Scene*>();

		std::string m_current_scene_name = "";
	};
}

#endif