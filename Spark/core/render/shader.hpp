#ifndef SPARK_SHADER_HPP
#define SPARK_SHADER_HPP

#include <core/pch.hpp>
#include <core/system/manager.hpp>
#include <include/glm/glm.hpp>

namespace Spark {
	enum class ShaderStage { VERTEX, FRAGMENT, COMPUTE, GEOMETRY, TESSELLATION };

	struct ShaderParam {
		std::any value;
		String   name;

		template<typename T> ShaderParam(const String& name, const T& value)
			: name(name)
			, value(value) {}
	};

	class ShaderParameters {
	  public:
		template<typename T> void Set(const String& name, const T& value) { m_parameters[name] = value; }

		template<typename T> T Get(const String& name) const {
			auto it = m_parameters.Find(name);
			if (it != m_parameters.End()) {
				return std::any_cast<T>(it->second);
			}
			return T();
		}

		bool Has(const String& name) const { return m_parameters.Find(name) != m_parameters.End(); }

		void Clear() { m_parameters.Clear(); }

		const UnorderedMap<String, std::any>& GetParams() const { return m_parameters; }

	  private:
		UnorderedMap<String, std::any> m_parameters;
	};

	// Generic shader interface
	class Shader {
	  public:
		virtual ~Shader()                                              = default;
		virtual void Bind()                                            = 0;
		virtual void Unbind()                                          = 0;

		// Generic parameter setting
		virtual void SetParameters(const ShaderParameters& params)     = 0;

		// Optional shader stages
		virtual void AddStage(ShaderStage stage, const String& source) = 0;
		virtual void Compile()                                         = 0;
	};
} // namespace Spark

#endif