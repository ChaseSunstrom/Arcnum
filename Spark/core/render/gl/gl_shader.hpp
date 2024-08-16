#ifndef SPARK_GL_SHADER_HPP
#define SPARK_GL_SHADER_HPP

#include <core/pch.hpp>
#include <core/render/shader.hpp>

namespace Spark
{
	class GLVertexShader : public VertexShader
	{
	public:
		friend class Manager<GLVertexShader>;
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
		friend class Manager<GLFragmentShader>;
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

	class GLRenderShader : public RenderShader
	{
	public:
		friend class Manager<GLRenderShader>;
		GLRenderShader(const std::filesystem::path& vertex_path, const std::filesystem::path& fragment_path)
			: RenderShader(vertex_path, fragment_path) {
			Compile();
		}
		~GLRenderShader();
		void Compile() override;
	private:
		u32 m_id;
	};
}


#endif