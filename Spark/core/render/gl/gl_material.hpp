#ifndef SPARK_GL_MATERIAL_HPP
#define SPARK_GL_MATERIAL_HPP

#include <core/pch.hpp>
#include <core/render/material.hpp>
#include <core/system/registry.hpp>

namespace Spark
{
	class GLMaterial : public Material
	{
	public:
		friend class Registry<GLMaterial>;
		u32 GetID() const { return m_id; }
	private:
		GLMaterial(const std::string& shader_name, u32 id, const glm::vec4& color, f32 diffuse, f32 specular, f32 ambient, f32 shininess) : Material(MaterialData{ color, diffuse, specular, ambient, shininess }, shader_name), m_id(id) {}
	private:
		// Shader program ID
		u32 m_id;
	};
}


#endif