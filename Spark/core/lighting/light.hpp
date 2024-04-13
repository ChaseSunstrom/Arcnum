#ifndef SPARK_LIGHT_HPP
#define SPARK_LIGHT_HPP

#include "../spark.hpp"

namespace spark
{
	struct dir_light_component
	{
		dir_light_component() = default;

		dir_light_component(
				math::vec3 direction, math::vec3 ambient, math::vec3 diffuse, math::vec3 specular) :
				m_direction(direction), m_ambient(ambient), m_diffuse(diffuse), m_specular(specular) { }

		bool operator!=(const dir_light_component& other) const
		{
			return m_direction != other.m_direction || m_ambient != other.m_ambient || m_diffuse != other.m_diffuse ||
			       m_specular != other.m_specular;
		}

		math::vec3 m_direction;

		math::vec3 m_ambient;

		math::vec3 m_diffuse;

		math::vec3 m_specular;
	};

	struct point_light_component
	{
		point_light_component() = default;

		point_light_component(
				math::vec3 position,
				f32 constant,
				f32 linear,
				f32 quadratic,
				math::vec3 ambient,
				math::vec3 diffuse,
				math::vec3 specular) :
				m_position(position), m_constant(constant), m_linear(linear), m_quadratic(quadratic), m_ambient(ambient), m_diffuse(
				diffuse), m_specular(specular) { }

		bool operator!=(const point_light_component& other) const
		{
			return m_position != other.m_position || m_constant != other.m_constant || m_linear != other.m_linear ||
			       m_quadratic != other.m_quadratic || m_ambient != other.m_ambient || m_diffuse != other.m_diffuse ||
			       m_specular != other.m_specular;
		}

		math::vec3 m_position;

		f32 m_constant;

		f32 m_linear;

		f32 m_quadratic;

		math::vec3 m_ambient;

		math::vec3 m_diffuse;

		math::vec3 m_specular;
	};

	struct spot_light_component
	{
		spot_light_component() = default;

		spot_light_component(
				math::vec3 position,
				math::vec3 direction,
				f32 cut_off,
				f32 outer_cut_off,
				f32 constant,
				f32 linear,
				f32 quadratic,
				math::vec3 ambient,
				math::vec3 diffuse,
				math::vec3 specular) :
				m_position(position), m_direction(direction), m_cut_off(cut_off), m_outer_cut_off(outer_cut_off), m_constant(
				constant), m_linear(linear), m_quadratic(quadratic), m_ambient(ambient), m_diffuse(diffuse), m_specular(
				specular) { }

		bool operator!=(const spot_light_component& other) const
		{
			return m_position != other.m_position || m_direction != other.m_direction || m_cut_off != other.m_cut_off ||
			       m_outer_cut_off != other.m_outer_cut_off || m_constant != other.m_constant ||
			       m_linear != other.m_linear || m_quadratic != other.m_quadratic || m_ambient != other.m_ambient ||
			       m_diffuse != other.m_diffuse || m_specular != other.m_specular;
		}

		math::vec3 m_position;

		math::vec3 m_direction;

		f32 m_cut_off;

		f32 m_outer_cut_off;

		f32 m_constant;

		f32 m_linear;

		f32 m_quadratic;

		math::vec3 m_ambient;

		math::vec3 m_diffuse;

		math::vec3 m_specular;
	};
}

#endif