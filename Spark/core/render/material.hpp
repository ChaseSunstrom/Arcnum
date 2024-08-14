#ifndef SPARK_MATERIAL_HPP
#define SPARK_MATERIAL_HPP

#include <core/pch.hpp>
#include <glm/vec4.hpp>

namespace Spark
{
	struct Material
	{
	public:
		Material(const glm::vec4& color, f32 diffuse, f32 specular, f32 ambient, f32 shiniess) : m_color(color), m_diffuse(diffuse), m_specular(specular), m_ambient(ambient), m_shiniess(shiniess) {}
		virtual ~Material() = default;
	protected:
		glm::vec4 m_color;
		f32 m_diffuse;
		f32 m_specular;
		f32 m_ambient;
		f32 m_shiniess;
		// TODO: TEXTURES
	};
}

#endif