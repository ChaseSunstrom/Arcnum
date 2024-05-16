#ifndef SPARK_VERTEX_HPP
#define SPARK_VERTEX_HPP

#include "../../spark.hpp"

namespace Spark
{
struct Vertex
{
    Vertex(const Math::vec3 &position = Math::vec3(0), const Math::vec3 &normal = Math::vec3(0),
           const Math::vec2 &texcoord = Math::vec2(0))
        : m_position(position), m_normal(normal), m_texcoords(texcoord)
    {
    }

    ~Vertex() = default;

    Math::vec3 m_position;

    Math::vec3 m_normal;

    Math::vec2 m_texcoords;
};
} // namespace Spark

#endif