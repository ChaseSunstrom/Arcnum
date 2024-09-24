#ifndef SPARK_SHADER_HPP
#define SPARK_SHADER_HPP

#include <core/pch.hpp>
#include <core/system/manager.hpp>
#include <include/glm/glm.hpp>

namespace Spark {
	enum class ShaderStage { VERTEX, FRAGMENT, COMPUTE, GEOMETRY, TESSELLATION };

	class IShader {
	  public:
		virtual void Compile()                                                          = 0;
		virtual void Use()                                                              = 0;
		// Convenience methods for specific types
		virtual void SetVec2(const String& name, const glm::vec2& value)           = 0;
		virtual void SetVec3(const String& name, const glm::vec3& value)           = 0;
		virtual void SetVec4(const String& name, const glm::vec4& value)           = 0;
		virtual void SetMat2(const String& name, const glm::mat2& value)           = 0;
		virtual void SetMat3(const String& name, const glm::mat3& value)           = 0;
		virtual void SetMat4(const String& name, const glm::mat4& value)           = 0;
		virtual void SetI32(const String& name, const i32 value)                   = 0;
		virtual void SetI64(const String& name, const i64 value)                   = 0;
		virtual void SetF32(const String& name, const f32 value)                   = 0;
		virtual void SetF64(const String& name, const f64 value)                   = 0;
		virtual void SetBool(const String& name, const bool value)                 = 0;
		virtual void BindUniformBlock(const String& block_name, u32 binding_point) = 0;
		virtual u32  GetUniformBlockIndex(const String& block_name)                = 0;
	};
	
	class RenderShader : public IShader {
	  public:
		friend class Manager<RenderShader>;
		virtual ~RenderShader()                                                           = default;
		virtual void AddShaderStage(ShaderStage stage, const std::filesystem::path& path) { m_shader_paths[stage] = path; }

	  protected:
		RenderShader(std::vector<std::pair<ShaderStage, const std::filesystem::path>> stage_paths) {
			for (const auto& [stage, path] : stage_paths)
				AddShaderStage(stage, path);
		}

	  protected:
		std::map<ShaderStage, std::filesystem::path> m_shader_paths;
	};
} // namespace Spark

#endif