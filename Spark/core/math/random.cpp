#include "random.hpp"
#include "../logging/log.hpp"

namespace Spark
{
namespace Math
{
u32 roll_dice(const u32 min, const u32 max)
{
    // Check if the range is valid
    if (min > max)
    {
        SPARK_ERROR("[RANDOM]: min must be less than or equal to max");
        return 0;
    }

    // Create a random number generator and seed it using random_device
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<u32> dist(min, max);

    // Generate a random number within the defined range (min to max)
    return dist(gen);
}

std::vector<u32> multi_dice_roll(const u32 min, const u32 max, const u32 num_dice)
{
    // Check if the range is valid
    if (min > max)
    {
        SPARK_ERROR("[RANDOM]: min must be less than or equal to max");
        return {};
    }

    std::vector<u32> rolled_dice(num_dice);
    std::generate(rolled_dice.begin(), rolled_dice.end(), [min, max]() { return roll_dice(min, max); });
    return rolled_dice;
}

std::vector<u32> roll_unfair_dice(const u32 min, const unsigned int max, const std::vector<f64> &percentages)
{
    // Check if the range is valid and the number of percentages matches the range
    if (min > max || percentages.size() != (max - min + 1))
    {
        SPARK_ERROR("[RANDOM]: Invalid input range or number of percentages");
        return {};
    }

    // Check if any percentage is less than 0 or the sum is not equal to 1
    auto invalid_percentage = std::any_of(percentages.begin(), percentages.end(), [](f64 p) { return p < 0; });
    f64 sum = std::accumulate(percentages.begin(), percentages.end(), 0.0);
    if (invalid_percentage || std::abs(sum - 1) > 1e-4)
    {
        SPARK_ERROR("[RANDOM]: Percentages must be non-negative and sum to 1");
        return {};
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<u32> dist(percentages.begin(), percentages.end());

    std::vector<u32> rolled_dice(percentages.size());
    std::transform(percentages.begin(), percentages.end(), rolled_dice.begin(),
                   [min, &dist, &gen](const auto &p) { return dist(gen) + min; });

    return rolled_dice;
}

Coin flip_coin()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::bernoulli_distribution dis(0.5);
    return dis(gen) ? Coin::HEADS : Coin::TAILS;
}

Coin flip_unfair_coin(const double percentage)
{
    // Check if percentage is within the range [0, 1]
    if (percentage < 0 || percentage > 1)
    {
        SPARK_ERROR("[RANDOM]: Percentage must be between 0 and 1");
        return Coin::TAILS;
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);
    return dis(gen) < percentage ? Coin::HEADS : Coin::TAILS;
}

} // namespace math
} // namespace Spark