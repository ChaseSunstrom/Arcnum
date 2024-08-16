#ifndef SPARK_MATERIAL_HPP
#define SPARK_MATERIAL_HPP

#include <core/pch.hpp>
#include <glm/vec4.hpp>

namespace Spark
{
	struct MaterialData
	{
		glm::vec4 m_color;
		f32 m_diffuse;
		f32 m_specular;
		f32 m_ambient;
		f32 m_shininess;
		// TODO: TEXTURES
	};

	struct Material
	{
	public:
		Material(const MaterialData& material_data, const std::string& shader_name) : m_material_data(material_data), m_shader_name(shader_name) {}
		virtual ~Material() = default;
	protected:
		MaterialData m_material_data;
		std::string m_shader_name;
	};
}

#endif