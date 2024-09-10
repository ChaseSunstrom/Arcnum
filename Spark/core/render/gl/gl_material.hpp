#ifndef SPARK_GL_MATERIAL_HPP
#define SPARK_GL_MATERIAL_HPP

#include <core/pch.hpp>
#include <core/render/material.hpp>
#include <core/util/gl.hpp>

namespace Spark {

	class GLMaterial : public Material {
	  public:
		GLMaterial(const std::string& name)
			: Material(name) {}

		void SetAlbedoTexture(u32 texture_id) { m_albedo_texture = texture_id; }
		void SetNormalTexture(u32 texture_id) { m_normal_texture = texture_id; }
		void SetMetallicRoughnessTexture(u32 texture_id) { m_metallic_roughness_texture = texture_id; }

		u32 GetAlbedoTexture() const { return m_albedo_texture; }
		u32 GetNormalTexture() const { return m_normal_texture; }
		u32 GetMetallicRoughnessTexture() const { return m_metallic_roughness_texture; }

		void ApplyToShader(RenderShader* shader) const override {
			// Set common material properties
			// TODO: Fix this shit
			shader->SetVec4("u_material.albedo", m_albedo);
			shader->SetF32("u_material.metallic", m_metallic);
			shader->SetF32("u_material.roughness", m_roughness);

			// Set texture uniforms
			i32 texture_unit = 0;
			if (m_albedo_texture != 0) {
				shader->SetI32("u_material.albedo_texture", texture_unit);
				glActiveTexture(GL_TEXTURE0 + texture_unit);
				glBindTexture(GL_TEXTURE_2D, m_albedo_texture);
				texture_unit++;
			}
			if (m_normal_texture != 0) {
				shader->SetI32("u_material.normal_texture", texture_unit);
				glActiveTexture(GL_TEXTURE0 + texture_unit);
				glBindTexture(GL_TEXTURE_2D, m_normal_texture);
				texture_unit++;
			}
			if (m_metallic_roughness_texture != 0) {
				shader->SetI32("u_material.metallic_roughness_texture", texture_unit);
				glActiveTexture(GL_TEXTURE0 + texture_unit);
				glBindTexture(GL_TEXTURE_2D, m_metallic_roughness_texture);
				texture_unit++;
			}

			// Set custom properties
			for (const auto& [name, value] : m_custom_properties) {
				if (value.type() == typeid(glm::vec2)) {
					shader->SetVec2(name, std::any_cast<glm::vec2>(value));
				} else if (value.type() == typeid(glm::vec3)) {
					shader->SetVec3(name, std::any_cast<glm::vec3>(value));
				} else if (value.type() == typeid(glm::vec4)) {
					shader->SetVec4(name, std::any_cast<glm::vec4>(value));
				} else if (value.type() == typeid(glm::mat4)) {
					shader->SetMat4(name, std::any_cast<glm::mat4>(value));
				} else if (value.type() == typeid(f32)) {
					shader->SetF32(name, std::any_cast<f32>(value));
				} else if (value.type() == typeid(i32)) {
					shader->SetI32(name, std::any_cast<i32>(value));
				} else if (value.type() == typeid(bool)) {
					shader->SetBool(name, std::any_cast<bool>(value));
				}
			}
		}

	  private:
		u32 m_albedo_texture             = 0;
		u32 m_normal_texture             = 0;
		u32 m_metallic_roughness_texture = 0;
	};

} // namespace Spark

#endif // SPARK_GL_MATERIAL_HPP