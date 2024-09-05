#ifndef SPARK_MATERIAL_HPP
#define SPARK_MATERIAL_HPP

#include <core/pch.hpp>
#include <core/render/shader.hpp>

namespace Spark {

	class Material {
	  public:
		Material(const std::string& name)
			: m_name(name)
			, m_custom_shader(nullptr)
			, m_albedo(1.0f)
			, m_metallic(0.0f)
			, m_roughness(0.5f) {}

		virtual ~Material() = default;

		void          SetCustomShader(RenderShader* shader) { m_custom_shader = shader; }
		RenderShader* GetCustomShader() const { return m_custom_shader; }
		bool          HasCustomShader() const { return m_custom_shader != nullptr; }

		// Common material properties
		void SetAlbedo(const glm::vec3& albedo) { m_albedo = albedo; }
		void SetMetallic(f32 metallic) { m_metallic = metallic; }
		void SetRoughness(f32 roughness) { m_roughness = roughness; }

		const glm::vec3& GetAlbedo() const { return m_albedo; }
		f32              GetMetallic() const { return m_metallic; }
		f32              GetRoughness() const { return m_roughness; }

		// Custom properties
		template<typename T> void SetProperty(const std::string& name, const T& value) { m_custom_properties[name] = value; }

		template<typename T> T GetProperty(const std::string& name) const {
			auto it = m_custom_properties.find(name);
			if (it != m_custom_properties.end()) {
				return std::any_cast<T>(it->second);
			}
			throw std::runtime_error("Property not found: " + name);
		}

		virtual void ApplyToShader(RenderShader* shader) const = 0;

	  protected:
		std::string   m_name;
		RenderShader* m_custom_shader;

		// Common material properties
		glm::vec3 m_albedo;
		f32       m_metallic;
		f32       m_roughness;

		// Custom properties
		std::unordered_map<std::string, std::any> m_custom_properties;
	};

} // namespace Spark

#endif // SPARK_MATERIAL_HPP