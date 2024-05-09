#ifndef SPARK_MATH_HPP
#define SPARK_MATH_HPP

#include "../spark.hpp"

namespace Spark
{
inline math::vec3 extract_position(const math::mat4 &transform)
{
    return math::vec3(transform[3][0], transform[3][1], transform[3][2]);
}
} // namespace Spark

#endif