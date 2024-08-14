#ifndef SPARK_SHADER_HPP
#define SPARK_SHADER_HPP

#include <core/pch.hpp>

namespace Spark
{
	// Note: We could have used a main IShader interface, but that would lead to a lot of virtual function calls
	//		 (Even more than we already will have) which are slow, and we also wont be able to accidently cast from one subtype to another
	//
	// Note: We could also have made the constructors public, but we want to make sure that they are stored in the Registry<T>
	class VertexShader
	{
	public:
		virtual ~VertexShader() = default;
		const std::filesystem::path& GetPath() const {
			return m_path;
		}
		virtual void Compile() = 0;
	protected:
		VertexShader(const std::filesystem::path& path) : m_path(path) {}
	protected:
		std::filesystem::path m_path;
	};

	class FragmentShader
	{
	public:
		virtual ~FragmentShader() = default;
		const std::filesystem::path& GetPath() const {
			return m_path;
		}
		virtual void Compile() = 0;
	protected:
		FragmentShader(const std::filesystem::path& path) : m_path(path) {}
	protected:
		std::filesystem::path m_path;
	};

	class ComputeShader
	{
	public:
		virtual ~ComputeShader() = default;
		const std::filesystem::path& GetPath() const {
			return m_path;
		}
		virtual void Compile() = 0;
	protected:
		ComputeShader(const std::filesystem::path& path) : m_path(path) {}
	protected:
		std::filesystem::path m_path;
	};

	class GeometryShader
	{
	public:
		virtual ~GeometryShader() = default;
		const std::filesystem::path& GetPath() const {
			return m_path;
		}
		virtual void Compile() = 0;
	protected:
		GeometryShader(const std::filesystem::path& path) : m_path(path) {}
	protected:
		std::filesystem::path m_path;
	};

	class TesselationShader
	{
	public:
		virtual ~TesselationShader() = default;
		const std::filesystem::path& GetPath() const {
			return m_path;
		}
		virtual void Compile() = 0;
	protected:
		TesselationShader(const std::filesystem::path& path) : m_path(path) {}
	protected:
		std::filesystem::path m_path;
	};

	// Basically just a VertexShader and FragmentShader
	class IRenderShader
	{
	public:
		virtual ~IRenderShader() = default;
	protected:
		IRenderShader() = default;
	};
}


#endif