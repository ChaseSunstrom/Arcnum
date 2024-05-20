#ifndef SPARK_SCENE_HPP
#define SPARK_SCENE_HPP

#include "../spark.hpp"

namespace Spark
{
struct SceneConfig
{
    SceneConfig() = default;

    SceneConfig(const Math::vec4 &background_color) : m_background_color(background_color)
    {
    }

    ~SceneConfig() = default;

    Math::vec4 m_background_color = Math::vec4(0);
};

class Scene
{
  public:
    Scene() = default;

    Scene(const SceneConfig &config) : m_config(const_cast<SceneConfig &>(config))
    {
    }

    ~Scene() = default;

    SceneConfig &get_scene_config() const
    {
        return m_config;
    }

    void set_background_color(const Math::vec4 &color)
    {
        m_config.m_background_color = color;
    }

  private:
    SceneConfig &m_config;
};
} // namespace Spark

#endif