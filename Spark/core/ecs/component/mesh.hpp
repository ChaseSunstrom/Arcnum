#ifndef SPARK_MESH_HPP
#define SPARK_MESH_HPP

#include "vertex.hpp"

namespace spark
{
	struct Mesh
	{
		Mesh(const std::vector <Vertex>& vertices, const std::vector <u32>& indices) :
				m_vertices(vertices), m_indices(indices) { }

		virtual ~Mesh() = default;

		virtual void update(const std::vector <Vertex>& vertices, const std::vector <u32>& indices) { }

		virtual void create_mesh() { }

		std::vector <Vertex> m_vertices = std::vector<Vertex>();

		std::vector <u32> m_indices = std::vector<u32>();
	};

}

#endif