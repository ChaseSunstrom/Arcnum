#ifndef SPARK_VERTEX_HPP
#define SPARK_VERTEX_HPP

#include <core/pch.hpp>
#include <include/glm/vec2.hpp>
#include <include/glm/vec3.hpp>

namespace Spark {
struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoords;
};
} // namespace Spark

#endif