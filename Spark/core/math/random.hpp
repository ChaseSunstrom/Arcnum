#ifndef SPARK_MATH_RANDOM_HPP
#define SPARK_MATH_RANDOM_HPP

#include "../spark.hpp"

namespace Spark
{
namespace math
{

u32 roll_dice(const u32 min, const u32 max);

std::vector<u32> multi_dice_roll(const u32 min, const u32 max, const u32 num_dice);

} // namespace math
} // namespace Spark

#endif // SPARK_MATH_RANDOM_HPP