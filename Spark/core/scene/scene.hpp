#ifndef SPARK_SCENE_HPP
#define SPARK_SCENE_HPP

#include "../ecs/ecs.hpp"
#include "../spark.hpp"
#include "../renderer/octree.hpp"

namespace spark
{
	struct scene_config
	{
		scene_config() = default;

		scene_config(const math::vec4& background_color) :
				m_background_color(background_color) { }

		~scene_config() = default;

		math::vec4 m_background_color = math::vec4(0);
	};

	class scene
	{
	public:
		scene() = default;

		scene(const scene_config& config) :
				m_config(const_cast<scene_config&>(config)) { }

		~scene() = default;

		scene_config& get_scene_config() const { return m_config; }
		octree& get_octree() const { return *m_octree; }

		void set_background_color(const math::vec4& color) { m_config.m_background_color = color; }
	private:
		scene_config& m_config;
		std::unique_ptr<octree> m_octree = std::make_unique<octree>(math::vec3(0), 1.0f);
	};
}

#endif