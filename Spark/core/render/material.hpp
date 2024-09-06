#ifndef SPARK_MATERIAL_HPP
#define SPARK_MATERIAL_HPP

#include <core/pch.hpp>
#include <core/math/math.hpp>
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
		void SetAlbedo(const Math::Vec4& albedo) { m_albedo = albedo; }
		void SetMetallic(f32 metallic) { m_metallic = metallic; }
		void SetRoughness(f32 roughness) { m_roughness = roughness; }

		const Math::Vec4& GetAlbedo() const { return m_albedo; }
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
		Math::Vec4 m_albedo;
		f32       m_metallic;
		f32       m_roughness;

		// Custom properties
		std::unordered_map<std::string, std::any> m_custom_properties;
	};

	template<> class Manager<Material> : public IManager {
	  public:
		Manager()
			: m_registry(std::make_unique<Registry<Material>>()) {}
		~Manager() = default;

		template<typename DerivedMaterial, typename... Args> DerivedMaterial& Create(const std::string& name, Args&&... args) {
			auto object = std::make_unique<DerivedMaterial>(name, std::forward<Args>(args)...);
			return static_cast<DerivedMaterial&>(Register(name, std::move(object)));
		}

		Material& Register(const std::string& name, std::unique_ptr<Material> object) { return m_registry->Register(name, std::move(object)); }

		Material&                Get(const std::string& name) const { return m_registry->Get(name); }
		Material&                Get(const Handle handle) const { return m_registry->Get(handle); }
		void                     Remove(const std::string& name) { m_registry->Remove(name); }
		void                     Remove(const Handle handle) { m_registry->Remove(handle); }
		size_t                   GetSize() const { return m_registry->GetSize(); }
		std::vector<std::string> GetKeys() const { return m_registry->GetKeys(); }
		void                     SetRegistry(std::unique_ptr<Registry<Material>> registry) { m_registry = std::move(registry); }
		Registry<Material>&      GetRegistry() const { return *m_registry; }

	  private:
		std::unique_ptr<Registry<Material>> m_registry;
	};

} // namespace Spark

#endif // SPARK_MATERIAL_HPP