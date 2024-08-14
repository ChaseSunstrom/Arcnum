#ifndef SPARK_GL_MESH_HPP
#define SPARK_GL_MESH_HPP

#include <core/pch.hpp>
#include <core/render/mesh.hpp>
#include <core/system/registry.hpp>

namespace Spark
{
	class GLMesh : public Mesh
	{
	public:
		friend class Registry<GLMesh>;
		void CreateMesh() override;
	private:
		GLMesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices) : Mesh(vertices, indices) {}
	private:
		u32 m_vao;
		u32 m_vbo;
		u32 m_ebo;
	};
}


#endif