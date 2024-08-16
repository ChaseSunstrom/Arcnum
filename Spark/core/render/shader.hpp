#ifndef SPARK_SHADER_HPP
#define SPARK_SHADER_HPP

#include <core/pch.hpp>
#include <core/system/manager.hpp>

namespace Spark
{

	// Note: We have made the constructors protected, but we want to make sure that they are stored in the Registry<T>
	class Shader
	{
	public:
		friend class Manager<Shader>;
		virtual ~Shader() = default;
		const std::filesystem::path& GetPath() const {
			return m_path;
		}
		virtual void Compile() = 0;
	protected:
		Shader(const std::filesystem::path& path) : m_path(path) {}
	protected:
		std::filesystem::path m_path;
	};

	class VertexShader : public Shader
	{
	public:
		friend class Manager<VertexShader>;
		void Compile() override = 0;
	protected:
		VertexShader(const std::filesystem::path& path) : Shader(path) {}
	};

	class FragmentShader : public Shader
	{
	public:
		friend class Manager<FragmentShader>;
		void Compile() override = 0;
	protected:
		FragmentShader(const std::filesystem::path& path) : Shader(path) {}
	};

	class ComputeShader : public Shader
	{
	public:
		friend class Manager<ComputeShader>;
		void Compile() override = 0;
	protected:
		ComputeShader(const std::filesystem::path& path) : Shader(path) {}
	};

	class GeometryShader : public Shader
	{
	public:
		friend class Manager<GeometryShader>;
		void Compile() override = 0;
	protected:
		GeometryShader(const std::filesystem::path& path) : Shader(path) {}
	};

	class TesselationShader : public Shader
	{
	public:
		friend class Manager<TesselationShader>;
		void Compile() override = 0;
	protected:
		TesselationShader(const std::filesystem::path& path) : Shader(path) {}
	};

	// Basically just a combination of a VertexShader and a FragmentShader
	class RenderShader : public Shader
	{
	public:
		friend class Manager<RenderShader>;
		virtual ~RenderShader() = default;
		void Compile() override = 0;
		const std::filesystem::path& GetVertexPath() const {
			return m_vertex_path;
		}
		const std::filesystem::path& GetFragmentPath() const {
			return m_fragment_path;
		}
	protected:
		RenderShader(const std::filesystem::path& vertex_path, const std::filesystem::path& fragment_path)
			: Shader(""), m_vertex_path(vertex_path), m_fragment_path(fragment_path) {}
	protected:
		std::filesystem::path m_vertex_path;
		std::filesystem::path m_fragment_path;
	};

	template <typename T>
	concept IsShader = std::derived_from<T, Shader>;
}


#endif