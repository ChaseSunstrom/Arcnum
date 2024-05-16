#ifndef SPARK_LIGHT_HPP
#define SPARK_LIGHT_HPP

#include "../spark.hpp"

namespace Spark
{
struct DirectionalLightComponent
{
    DirectionalLightComponent() = default;

    DirectionalLightComponent(Math::vec3 direction, Math::vec3 ambient, Math::vec3 diffuse, Math::vec3 specular)
        : m_direction(direction), m_ambient(ambient), m_diffuse(diffuse), m_specular(specular)
    {
    }

    bool operator!=(const DirectionalLightComponent &other) const
    {
        return m_direction != other.m_direction || m_ambient != other.m_ambient || m_diffuse != other.m_diffuse ||
               m_specular != other.m_specular;
    }

    Math::vec3 m_direction;

    Math::vec3 m_ambient;

    Math::vec3 m_diffuse;

    Math::vec3 m_specular;
};

struct PointLightComponent
{
    PointLightComponent() = default;

    PointLightComponent(Math::vec3 position, f32 constant, f32 linear, f32 quadratic, Math::vec3 ambient,
                        Math::vec3 diffuse, Math::vec3 specular)
        : m_position(position), m_constant(constant), m_linear(linear), m_quadratic(quadratic), m_ambient(ambient),
          m_diffuse(diffuse), m_specular(specular)
    {
    }

    bool operator!=(const PointLightComponent &other) const
    {
        return m_position != other.m_position || m_constant != other.m_constant || m_linear != other.m_linear ||
               m_quadratic != other.m_quadratic || m_ambient != other.m_ambient || m_diffuse != other.m_diffuse ||
               m_specular != other.m_specular;
    }

    Math::vec3 m_position;

    f32 m_constant;

    f32 m_linear;

    f32 m_quadratic;

    Math::vec3 m_ambient;

    Math::vec3 m_diffuse;

    Math::vec3 m_specular;
};

struct SpotLightComponent
{
    SpotLightComponent() = default;

    SpotLightComponent(Math::vec3 position, Math::vec3 direction, f32 cut_off, f32 outer_cut_off, f32 constant,
                       f32 linear, f32 quadratic, Math::vec3 ambient, Math::vec3 diffuse, Math::vec3 specular)
        : m_position(position), m_direction(direction), m_cut_off(cut_off), m_outer_cut_off(outer_cut_off),
          m_constant(constant), m_linear(linear), m_quadratic(quadratic), m_ambient(ambient), m_diffuse(diffuse),
          m_specular(specular)
    {
    }

    bool operator!=(const SpotLightComponent &other) const
    {
        return m_position != other.m_position || m_direction != other.m_direction || m_cut_off != other.m_cut_off ||
               m_outer_cut_off != other.m_outer_cut_off || m_constant != other.m_constant ||
               m_linear != other.m_linear || m_quadratic != other.m_quadratic || m_ambient != other.m_ambient ||
               m_diffuse != other.m_diffuse || m_specular != other.m_specular;
    }

    Math::vec3 m_position;

    Math::vec3 m_direction;

    f32 m_cut_off;

    f32 m_outer_cut_off;

    f32 m_constant;

    f32 m_linear;

    f32 m_quadratic;

    Math::vec3 m_ambient;

    Math::vec3 m_diffuse;

    Math::vec3 m_specular;
};
} // namespace Spark

#endif