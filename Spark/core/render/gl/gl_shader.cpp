#include "gl_shader.hpp"
#include <core/util/file.hpp>
#include <core/util/gl.hpp>
#include <core/util/log.hpp>

namespace Spark
{
GLVertexShader::GLVertexShader(const std::filesystem::path &path) : VertexShader(path) { Compile(); }

GLFragmentShader::GLFragmentShader(const std::filesystem::path &path) : FragmentShader(path) { Compile(); }

GLComputeShader::GLComputeShader(const std::filesystem::path &path) : ComputeShader(path) { Compile(); }

GLRenderShader::~GLRenderShader()
{
	if (m_id != 0)
	{
		glDeleteProgram(m_id);
	}
}

void GLRenderShader::AddShaderStage(ShaderStage stage, const std::filesystem::path &path) { m_shader_paths[stage] = path; }

GLVertexShader::~GLVertexShader() { glDeleteShader(m_id); }

GLFragmentShader::~GLFragmentShader() { glDeleteShader(m_id); }

GLComputeShader::~GLComputeShader() { glDeleteShader(m_id); }
void GLVertexShader::Compile() { m_id = GLShaderCommon::CompileGLShaderInternal(m_path, GL_VERTEX_SHADER); }

void GLFragmentShader::Compile() { m_id = GLShaderCommon::CompileGLShaderInternal(m_path, GL_FRAGMENT_SHADER); }

void GLComputeShader::Compile() { m_id = GLShaderCommon::CompileGLShaderInternal(m_path, GL_COMPUTE_SHADER); }

void GLRenderShader::Compile()
{
	m_id = glCreateProgram();
	std::vector<u32> shader_ids;

	for (const auto &[stage, path] : m_shader_paths)
	{
		GLenum gl_shader_type;
		switch (stage)
		{
		case ShaderStage::Vertex:
			gl_shader_type = GL_VERTEX_SHADER;
			break;
		case ShaderStage::Fragment:
			gl_shader_type = GL_FRAGMENT_SHADER;
			break;
		case ShaderStage::Compute:
			gl_shader_type = GL_COMPUTE_SHADER;
			break;
		case ShaderStage::Geometry:
			gl_shader_type = GL_GEOMETRY_SHADER;
			break;
		case ShaderStage::Tessellation:
			gl_shader_type = GL_TESS_CONTROL_SHADER;
			break; // Note: You might need to handle both control and evaluation
				   // shaders
		default:
			LOG_ERROR("Unsupported shader stage");
			continue;
		}

		u32 shader_id = GLShaderCommon::CompileGLShaderInternal(path, gl_shader_type);
		glAttachShader(m_id, shader_id);
		shader_ids.push_back(shader_id);
	}

	glLinkProgram(m_id);
	GLShaderCommon::CheckShaderSuccess(m_id);

	// Detach and delete individual shaders
	for (u32 shader_id : shader_ids)
	{
		glDetachShader(m_id, shader_id);
		glDeleteShader(shader_id);
	}
}
} // namespace Spark