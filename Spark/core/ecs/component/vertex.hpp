#ifndef SPARK_VERTEX_HPP
#define SPARK_VERTEX_HPP

#include "../../spark.hpp"

namespace spark
{
	struct Vertex
	{
		Vertex(
				const math::vec3& position = math::vec3(0),
				const math::vec3& normal = math::vec3(0),
				const math::vec2& texcoord = math::vec2(0)) :
				m_position(position), m_normal(normal), m_texcoords(texcoord) { }

		~Vertex() = default;

		math::vec3 m_position;

		math::vec3 m_normal;

		math::vec2 m_texcoords;
	};
}

#endif