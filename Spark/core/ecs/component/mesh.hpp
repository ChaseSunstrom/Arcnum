#ifndef SPARK_MESH_HPP
#define SPARK_MESH_HPP

#include "vertex.hpp"

namespace spark
{
	struct mesh
	{
		mesh(const std::vector <vertex>& vertices, const std::vector <u32>& indices) :
				m_vertices(vertices), m_indices(indices) { }

		virtual ~mesh() = default;

		virtual void update(const std::vector <vertex>& vertices, const std::vector <u32>& indices) { }

		virtual void create_mesh() { }

		std::vector <vertex> m_vertices = std::vector<vertex>();

		std::vector <u32> m_indices = std::vector<u32>();
	};

}

#endif