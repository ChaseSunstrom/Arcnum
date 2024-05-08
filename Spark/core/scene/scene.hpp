#ifndef SPARK_SCENE_HPP
#define SPARK_SCENE_HPP

#include "../ecs/ecs.hpp"
#include "../spark.hpp"

namespace Spark {
struct SceneConfig {
  SceneConfig() = default;

  SceneConfig(const math::vec4 &background_color)
      : m_background_color(background_color) {}

  ~SceneConfig() = default;

  math::vec4 m_background_color = math::vec4(0);
};

class Scene {
public:
  Scene() = default;

  Scene(const SceneConfig &config)
      : m_config(const_cast<SceneConfig &>(config)) {}

  ~Scene() = default;

  SceneConfig &get_scene_config() const { return m_config; }

  void set_background_color(const math::vec4 &color) {
    m_config.m_background_color = color;
  }

private:
  SceneConfig &m_config;
};
} // namespace spark

#endif