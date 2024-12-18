#ifndef SPARK_MATERIAL_HPP
#define SPARK_MATERIAL_HPP

#include "shader.hpp"

namespace Spark {
	class Material {
	  public:
		Material(const String& name)
			: m_name(name) {}

		virtual ~Material() = default;

		void SetShader(RefPtr<Shader> shader) { m_shader = shader; }

		RefPtr<Shader> GetShader() const { return m_shader; }

		// Generic parameter setting
		template<typename T> void SetParameter(const String& name, const T& value) { m_parameters.Set(name, value); }

		// Add texture binding
		void BindTexture(const String& name, u32 slot, u32 textureId) { m_textures[name] = {slot, textureId}; }

		virtual void Apply() = 0;

	  protected:
		String           m_name;
		RefPtr<Shader>   m_shader;
		ShaderParameters m_parameters;

		struct TextureBinding {
			u32 slot;
			u32 textureId;
		};
		UnorderedMap<String, TextureBinding> m_textures;
	};

} // namespace Spark

#endif