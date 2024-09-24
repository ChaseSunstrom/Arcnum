#ifndef SPARK_MATERIAL_HPP
#define SPARK_MATERIAL_HPP

#include <core/pch.hpp>
#include <core/math/math.hpp>
#include <core/util/memory/ref_ptr.hpp>
#include <core/render/shader.hpp>

namespace Spark {

	class Material {
	  public:
		Material(const String& name)
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
		void SetAlbedo(const _MATH Vec4& albedo) { m_albedo = albedo; }
		void SetMetallic(f32 metallic) { m_metallic = metallic; }
		void SetRoughness(f32 roughness) { m_roughness = roughness; }

		const _MATH Vec4& GetAlbedo() const { return m_albedo; }
		f32              GetMetallic() const { return m_metallic; }
		f32              GetRoughness() const { return m_roughness; }

		// Custom properties
		template<typename _Ty> void SetProperty(const String& name, const _Ty& value) { m_custom_properties[name] = value; }

		template<typename _Ty> _Ty GetProperty(const String& name) const {
			auto it = m_custom_properties.Find(name);
			if (it != m_custom_properties.End()) {
				return std::any_cast<_Ty>(it->second);
			}
			LOG_FATAL("Property not found: " << name);
		}

		virtual void ApplyToShader(RenderShader* shader) const = 0;

	  protected:
		String   m_name;
		RenderShader* m_custom_shader;

		// Common material properties
		_MATH Vec4 m_albedo;
		f32       m_metallic;
		f32       m_roughness;

		// Custom properties
		UnorderedMap<String, std::any> m_custom_properties;
	};

	template<> class Manager<Material> : public IManager {
	  public:
		Manager()
			: m_registry(MakeUnique<Registry<Material>>()) {}
		~Manager() = default;

		template<typename DerivedMaterial, typename... Args> RefPtr<DerivedMaterial> Create(const String& name, Args&&... args) {
			auto object = MakeUnique<DerivedMaterial>(name, Forward<Args>(args)...);
			return RefCast<DerivedMaterial>(Register(name, Move(object)));
		}

		RefPtr<Material> Register(const String& name, UniquePtr<Material> object) { return m_registry->Register(name, Move(object)); }

		RefPtr<Material>                Get(const String& name) const { return m_registry->Get(name); }
		RefPtr<Material>                Get(const Handle handle) const { return m_registry->Get(handle); }
		void                     Remove(const String& name) { m_registry->Remove(name); }
		void                     Remove(const Handle handle) { m_registry->Remove(handle); }
		size_t                    GetSize() const { return m_registry->GetSize(); }
		Vector<String> GetKeys() const { return m_registry->GetKeys(); }
		void                     SetRegistry(UniquePtr<Registry<Material>> registry) { m_registry = Move(registry); }
		Registry<Material>&      GetRegistry() const { return *m_registry; }

	  private:
		UniquePtr<Registry<Material>> m_registry;
	};

} // namespace Spark

#endif // SPARK_MATERIAL_HPP