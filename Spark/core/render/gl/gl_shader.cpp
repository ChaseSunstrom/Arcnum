#include "gl_shader.hpp"
#include <core/util/file.hpp>
#include <core/util/log.hpp>
#include <core/util/gl.hpp>

namespace Spark
{
	static void CheckShaderSuccess(u32 shader)
	{
		i32 success;
		char info[1024];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(shader, sizeof(info), nullptr, info);
			LOG_ERROR("Failed to compile vertex shader: " << info);
			glDeleteShader(shader);
		}
	}

	static u32 CompileGLShaderInternal(const std::filesystem::path& path, u32 type)
	{
		const char* code = ReadFile(path).c_str();
		u32 shader = glCreateShader(type);

		glShaderSource(shader, 1, &code, nullptr);
		glCompileShader(shader);

		CheckShaderSuccess(shader);

		return shader;
	}

	GLVertexShader::~GLVertexShader()
	{
		glDeleteShader(m_id);
	}

	GLFragmentShader::~GLFragmentShader()
	{
		glDeleteShader(m_id);
	}

	GLRenderShader::~GLRenderShader()
	{
		glDeleteShader(m_id);
	}

	void GLVertexShader::Compile()
	{
		m_id = CompileGLShaderInternal(m_path, GL_VERTEX_SHADER);
	}

	void GLFragmentShader::Compile()
	{
		m_id = CompileGLShaderInternal(m_path, GL_FRAGMENT_SHADER);
	}

	GLRenderShader::GLRenderShader(const std::filesystem::path& vertex_path, const std::filesystem::path& fragment_path)
	{
		GLVertexShader vertex_shader(vertex_path);
		GLFragmentShader fragment_shader(fragment_path);

		vertex_shader.Compile();
		fragment_shader.Compile();

		Compile(vertex_shader.GetId(), fragment_shader.GetId());
	}

	void GLRenderShader::Compile(u32 vertex_id, u32 fragment_id)
	{
		i32 shader = glCreateProgram();
		glAttachShader(shader, vertex_id);
		glAttachShader(shader, fragment_id);
		glLinkProgram(shader);

		CheckShaderSuccess(shader);

		glDetachShader(shader, vertex_id);
		glDetachShader(shader, fragment_id);

		m_id = shader;
	}
}