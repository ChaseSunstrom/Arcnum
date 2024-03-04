#ifndef SPARK_SCENE_HPP
#define SPARK_SCENE_HPP

#include "../ecs/ecs.hpp"
#include "../spark.hpp"

namespace spark
{
	struct scene_config
	{
		scene_config() = default;

		scene_config(const glm::vec4& background_color) :
				m_background_color(background_color) { }

		~scene_config() = default;

		glm::vec4 m_background_color = glm::vec4(0);
	};

	class scene
	{
	public:
		scene() = default;

		scene(const scene_config& config) :
				m_config(const_cast<scene_config&>(config)) { }

		~scene() = default;

		ecs& get_ecs() const { return *m_ecs.get(); }

		scene_config& get_scene_config() const { return m_config; }

		void set_background_color(const glm::vec4& color) { m_config.m_background_color = color; }
	private:
		// This is a shared pointer because multiple scenes can share the same ECS
		std::shared_ptr <ecs> m_ecs = std::make_shared<ecs>();

		// Same with this
		scene_config& m_config;
	};
}

#endif