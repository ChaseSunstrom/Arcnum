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
		// Shader program ID
		friend class Registry<GLMaterial>;
		u32 GetID() const { return m_id; }
	private:
		GLMaterial(u32 id, const glm::vec4& color, f32 diffuse, f32 specular, f32 ambient, f32 shininess) : Material(color, diffuse, specular, ambient, shininess), m_id(id) {}
	private:
		u32 m_id;
	};
}


#endif