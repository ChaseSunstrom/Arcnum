// gl_shader.hpp
#ifndef SPARK_GL_SHADER_HPP
#define SPARK_GL_SHADER_HPP

#include <core/pch.hpp>
#include <core/render/shader.hpp>
#include <core/util/classic/file.hpp>
#include <core/util/gl.hpp>

namespace Spark {
	class GLShader : public Shader {
	  public:
		GLShader(const String& name)
			: m_name(name) {}

		~GLShader() override {
			if (m_program_id) {
				glDeleteProgram(m_program_id);
			}
			for (auto [stage, id] : m_stage_ids) {
				glDeleteShader(id);
			}
		}

		void Bind() override { glUseProgram(m_program_id); }

		void Unbind() override { glUseProgram(0); }

		void AddStage(ShaderStage stage, const String& source) override {
			GLuint shader_id = CompileShaderStage(stage, source);
			if (shader_id) {
				m_stage_ids[stage] = shader_id;
				m_sources[stage]   = source;
			}
		}

		void Compile() override {
			m_program_id = glCreateProgram();

			// Attach all shader stages
			for (const auto& [stage, shader_id] : m_stage_ids) {
				glAttachShader(m_program_id, shader_id);
			}

			// Link program
			glLinkProgram(m_program_id);

			// Check linking status
			GLint success;
			glGetProgramiv(m_program_id, GL_LINK_STATUS, &success);
			if (!success) {
				GLchar info_log[512];
				glGetProgramInfoLog(m_program_id, sizeof(info_log), nullptr, info_log);
				LOG_ERROR("Shader program linking failed: " << info_log);
				return;
			}

			// Detach and delete shader stages after successful linking
			for (const auto& [stage, shader_id] : m_stage_ids) {
				glDetachShader(m_program_id, shader_id);
				glDeleteShader(shader_id);
			}
			m_stage_ids.Clear();
		}

		void SetParameters(const ShaderParameters& params) override {
			for (const auto& [name, value] : params.GetParams()) {
				SetUniform(name, value);
			}
		}

	  private:
		GLuint CompileShaderStage(ShaderStage stage, const String& source) {
			GLenum gl_stage = GetGLShaderStage(stage);
			if (gl_stage == 0) {
				LOG_ERROR("Invalid shader stage");
				return 0;
			}

			GLuint      shader_id  = glCreateShader(gl_stage);
			const char* source_str = source.CStr();
			glShaderSource(shader_id, 1, &source_str, nullptr);
			glCompileShader(shader_id);

			// Check compilation status
			GLint success;
			glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
			if (!success) {
				GLchar info_log[512];
				glGetShaderInfoLog(shader_id, sizeof(info_log), nullptr, info_log);
				LOG_ERROR("Shader compilation failed: " << info_log);
				glDeleteShader(shader_id);
				return 0;
			}

			return shader_id;
		}

		static GLenum GetGLShaderStage(ShaderStage stage) {
			switch (stage) {
				case ShaderStage::VERTEX:
					return GL_VERTEX_SHADER;
				case ShaderStage::FRAGMENT:
					return GL_FRAGMENT_SHADER;
				case ShaderStage::COMPUTE:
					return GL_COMPUTE_SHADER;
				case ShaderStage::GEOMETRY:
					return GL_GEOMETRY_SHADER;
				case ShaderStage::TESSELLATION:
					return GL_TESS_CONTROL_SHADER;
				default:
					return 0;
			}
		}

		void SetUniform(const String& name, const std::any& value) {
			GLint location = glGetUniformLocation(m_program_id, name.CStr());
			if (location == -1)
				return;

			try {
				if (value.type() == typeid(f32)) {
					glUniform1f(location, std::any_cast<f32>(value));
				} else if (value.type() == typeid(i32)) {
					glUniform1i(location, std::any_cast<i32>(value));
				} else if (value.type() == typeid(glm::vec2)) {
					const auto& v = std::any_cast<glm::vec2>(value);
					glUniform2f(location, v.x, v.y);
				} else if (value.type() == typeid(glm::vec3)) {
					const auto& v = std::any_cast<glm::vec3>(value);
					glUniform3f(location, v.x, v.y, v.z);
				} else if (value.type() == typeid(glm::vec4)) {
					const auto& v = std::any_cast<glm::vec4>(value);
					glUniform4f(location, v.x, v.y, v.z, v.w);
				} else if (value.type() == typeid(glm::mat4)) {
					const auto& m = std::any_cast<glm::mat4>(value);
					glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m));
				}
			} catch (const std::bad_any_cast&) {
				LOG_ERROR("Invalid uniform type for: " << name);
			}
		}

	  private:
		String                            m_name;
		u32                               m_program_id = 0;
		UnorderedMap<ShaderStage, u32>    m_stage_ids;
		UnorderedMap<ShaderStage, String> m_sources;
	};
} // namespace Spark

#endif