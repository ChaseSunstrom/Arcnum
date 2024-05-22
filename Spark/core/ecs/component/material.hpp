#ifndef SPARK_MATERIAL_HPP
#define SPARK_MATERIAL_HPP

#include "../../spark.hpp"
#include "texture.hpp"

namespace Spark
{

struct Material
{
    Material(const Math::vec4 &color, Texture &tex, const std::vector<std::string>& shader_paths, i32 diffuse, i32 specular, i32 ambient, f32 shininess)
        : m_color(color), m_diffuse(diffuse), m_specular(specular), m_ambient(ambient), m_shininess(shininess),
          m_texture(tex), m_shader_paths()
    {}

    ~Material() = default;

    Math::vec4 m_color;

    i32 m_diffuse;

    i32 m_specular;

    i32 m_ambient;

    f32 m_shininess;

    Texture &m_texture;
};
}

#endif