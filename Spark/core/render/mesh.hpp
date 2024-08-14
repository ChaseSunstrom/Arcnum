#ifndef SPARK_MESH_HPP
#define SPARK_MESH_HPP

#include <core/pch.hpp>
#include "vertex.hpp"

namespace Spark
{
	class Mesh
	{
	public:
		Mesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices) : m_vertices(vertices), m_indices(indices) {}
		virtual ~Mesh() = default;
		virtual void CreateMesh() = 0;
		const std::vector<Vertex>& GetVertices() const { return m_vertices; }
		const std::vector<u32>& GetIndices() const { return m_indices; }
	protected:
		std::vector<Vertex> m_vertices;
		std::vector<u32> m_indices;
	};
}


#endif