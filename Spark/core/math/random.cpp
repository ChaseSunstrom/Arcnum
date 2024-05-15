#include "random.hpp"

namespace Spark
{
namespace math
{
u32 roll_dice(const u32 min, const u32 max)
{
    // Create a random number generator and seed it using random_device
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<u32> dist(min, max);

    // Generate a random number within the defined range (min to max)
    return static_cast<u32>(dist(gen));
}

std::vector<u32> multi_dice_roll(const u32 min, const u32 max, const u32 num_dice)
{
    std::vector<u32> rolled_dice;

    for (u32 i = 0; i <= num_dice; ++i)
    {
        rolled_dice.push_back(roll_dice(min, max));
    }

    return rolled_dice;
}
} // namespace math
} // namespace Spark