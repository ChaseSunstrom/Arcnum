#ifndef SPARK_Math_HPP
#define SPARK_Math_HPP

#include "../spark.hpp"

namespace Spark
{
inline Math::vec3 extract_position(const Math::mat4 &transform)
{
    return Math::vec3(transform[3][0], transform[3][1], transform[3][2]);
}
} // namespace Spark

#endif