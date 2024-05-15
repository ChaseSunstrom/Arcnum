#ifndef SPARK_MATH_RANDOM_HPP
#define SPARK_MATH_RANDOM_HPP

#include "../spark.hpp"

namespace Spark
{
namespace math
{

enum class CoinFlip
{
  TAILS = 0,
  HEADS = 1
}

u32 roll_dice(const u32 min, const u32 max);

std::vector<u32> multi_dice_roll(const u32 min, const u32 max, const u32 num_dice);

std::vector<u32> roll_unfair_dice(const u32 min, const u32 max, const std::vector<f64>& percentages);

CoinFlip flip_coin();

CoinFlip flip_unfair_coin(const f64 percentage);

} // namespace math
} // namespace Spark

#endif // SPARK_MATH_RANDOM_HPP
