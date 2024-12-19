#ifndef SPARK_MATERIAL_HPP
#define SPARK_MATERIAL_HPP

#include "shader.hpp"
#include "core/resource/asset.hpp"

namespace Spark {
	class Material : public Asset<Material> {
	  public:
		Material(const String& name)
			: Asset(name) {}

		virtual ~Material() = default;

		void SetShader(RefPtr<Shader> shader) { m_shader = shader; }

		RefPtr<Shader> GetShader() const { return m_shader; }

		// Generic parameter setting
		template<typename T> void SetParameter(const String& name, const T& value) { m_parameters.Set(name, value); }

		// Add texture binding
		void BindTexture(const String& name, u32 slot, u32 texture_id) { m_textures[name] = {slot, texture_id}; }

		virtual void Apply() = 0;

	  protected:
		RefPtr<Shader>   m_shader;
		ShaderParameters m_parameters;

		struct TextureBinding {
			u32 slot;
			u32 texture_id;
		};
		UnorderedMap<String, TextureBinding> m_textures;
	};

} // namespace Spark

#endif