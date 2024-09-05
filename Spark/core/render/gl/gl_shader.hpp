#ifndef SPARK_GL_SHADER_HPP
#define SPARK_GL_SHADER_HPP

#include <core/pch.hpp>
#include <core/render/shader.hpp>
#include <core/util/file.hpp>
#include <core/util/gl.hpp>

namespace Spark {
	class GLShaderCommon {
	  protected:
		static void Use(u32 id) { glUseProgram(id); }
		static void SetVec2(u32 id, const std::string& name, const glm::vec2& value) { glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]); }
		static void SetVec3(u32 id, const std::string& name, const glm::vec3& value) { glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]); }
		static void SetVec4(u32 id, const std::string& name, const glm::vec4& value) { glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]); }
		static void SetMat2(u32 id, const std::string& name, const glm::mat2& value) { glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &value[0][0]); }
		static void SetMat3(u32 id, const std::string& name, const glm::mat3& value) { glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &value[0][0]); }
		static void SetMat4(u32 id, const std::string& name, const glm::mat4& value) { glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &value[0][0]); }
		static void SetI32(u32 id, const std::string& name, const i32 value) { glUniform1i(glGetUniformLocation(id, name.c_str()), value); }
		static void SetI64(u32 id, const std::string& name, const i64 value) { glUniform1i(glGetUniformLocation(id, name.c_str()), static_cast<GLint>(value)); }
		static void SetF32(u32 id, const std::string& name, const f32 value) { glUniform1f(glGetUniformLocation(id, name.c_str()), value); }
		static void SetF64(u32 id, const std::string& name, const f64 value) { glUniform1f(glGetUniformLocation(id, name.c_str()), static_cast<GLfloat>(value)); }
		static void SetBool(u32 id, const std::string& name, const bool value) { glUniform1i(glGetUniformLocation(id, name.c_str()), static_cast<GLint>(value)); }
		static void BindUniformBlock(u32 id, const std::string& block_name, u32 binding_point) {
			u32 block_index = glGetUniformBlockIndex(id, block_name.c_str());
			if (block_index != GL_INVALID_INDEX) {
				glUniformBlockBinding(id, block_index, binding_point);
			}
		}
		static u32  GetUniformBlockIndex(u32 id, const std::string& block_name) { return glGetUniformBlockIndex(id, block_name.c_str()); }
		static void CheckShaderSuccess(u32 shader) {
			i32  success;
			char info[1024];
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

			if (!success) {
				glGetShaderInfoLog(shader, sizeof(info), nullptr, info);
				glDeleteShader(shader);
				LOG_FATAL("Failed to compile vertex shader: " << info);
			}
		}
		static u32 CompileGLShaderInternal(const std::filesystem::path& path, u32 type) {
			std::string code   = ReadFile(path);
			u32         shader = glCreateShader(type);
			const char* c_str  = code.c_str();

			glShaderSource(shader, 1, &c_str, nullptr);
			glCompileShader(shader);

			CheckShaderSuccess(shader);

			return shader;
		}
	};

	class GLRenderShader
		: public RenderShader
		, private GLShaderCommon {
	  public:
		friend class Manager<GLRenderShader>;
		GLRenderShader(const std::vector<std::pair<ShaderStage, const std::filesystem::path>>& stage_paths)
			: RenderShader(stage_paths) {}
		~GLRenderShader();
		u32  GetId() const { return m_id; }
		void Compile() override;
		void Use() override { GLShaderCommon::Use(m_id); }
		void SetVec2(const std::string& name, const glm::vec2& value) override { GLShaderCommon::SetVec2(m_id, name, value); }
		void SetVec3(const std::string& name, const glm::vec3& value) override { GLShaderCommon::SetVec3(m_id, name, value); }
		void SetVec4(const std::string& name, const glm::vec4& value) override { GLShaderCommon::SetVec4(m_id, name, value); }
		void SetMat2(const std::string& name, const glm::mat2& value) override { GLShaderCommon::SetMat2(m_id, name, value); }
		void SetMat3(const std::string& name, const glm::mat3& value) override { GLShaderCommon::SetMat3(m_id, name, value); }
		void SetMat4(const std::string& name, const glm::mat4& value) override { GLShaderCommon::SetMat4(m_id, name, value); }
		void SetI32(const std::string& name, const i32 value) override { GLShaderCommon::SetI32(m_id, name, value); }
		void SetI64(const std::string& name, const i64 value) override { GLShaderCommon::SetI64(m_id, name, value); }
		void SetF32(const std::string& name, const f32 value) override { GLShaderCommon::SetF32(m_id, name, value); }
		void SetF64(const std::string& name, const f64 value) override { GLShaderCommon::SetF64(m_id, name, value); }
		void SetBool(const std::string& name, const bool value) override { GLShaderCommon::SetBool(m_id, name, value); }
		void BindUniformBlock(const std::string& block_name, u32 binding_point) override { GLShaderCommon::BindUniformBlock(m_id, block_name, binding_point); }
		u32  GetUniformBlockIndex(const std::string& block_name) override { return GLShaderCommon::GetUniformBlockIndex(m_id, block_name); }
		void AddShaderStage(ShaderStage stage, const std::filesystem::path& path) override;

	  private:
		u32 m_id = 0;
	};
} // namespace Spark

#endif