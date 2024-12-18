#ifndef SPARK_GL_MATERIAL_HPP
#define SPARK_GL_MATERIAL_HPP

#include <core/pch.hpp>
#include <core/render/material.hpp>
#include <core/util/gl.hpp>

namespace Spark {
	class GLMaterial : public Material {
	  public:
		GLMaterial(const String& name)
			: Material(name) {}

		// No predefined parameters or textures - everything is user defined
		void Apply() override {
			if (!m_shader)

				return;

			m_shader->Bind();

			// Set all parameters

			m_shader->SetParameters(m_parameters);

			// Bind all textures

			for (const auto& [name, binding] : m_textures) {
				glActiveTexture(GL_TEXTURE0 + binding.slot);

				glBindTexture(GL_TEXTURE_2D, binding.textureId);

				SetParameter(name, binding.slot); // Set the sampler uniform to the texture slot
			}
		}
	};
} // namespace Spark

#endif // SPARK_GL_MATERIAL_HPP