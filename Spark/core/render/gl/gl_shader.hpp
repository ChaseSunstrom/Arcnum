#ifndef SPARK_GL_SHADER_HPP
#define SPARK_GL_SHADER_HPP

#include <core/pch.hpp>
#include <core/render/shader.hpp>

namespace Spark
{
	class GLVertexShader : public VertexShader
	{
	public:
		GLVertexShader(const std::filesystem::path& path) : VertexShader(path) {
			Compile();
		}
		~GLVertexShader();
		u32 GetId() const {
			return m_id;
		}
		void Compile() override;
	private:
		u32 m_id;
	};

	class GLFragmentShader : public FragmentShader
	{
	public:
		GLFragmentShader(const std::filesystem::path& path) : FragmentShader(path) {
			Compile();
		}
		~GLFragmentShader();
		u32 GetId() const {
			return m_id;
		}
		void Compile() override;
	private:
		u32 m_id;
	};

	class GLRenderShader : public IRenderShader
	{
	public:
		GLRenderShader(const std::filesystem::path& vertex_path, const std::filesystem::path& fragment_path);
		~GLRenderShader();
	private:
		void Compile(u32 vertex_id, u32 fragment_id);
	private:
		u32 m_id;
	};
}


#endif