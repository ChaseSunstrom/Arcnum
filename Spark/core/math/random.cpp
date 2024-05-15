#include "random.hpp"
#include "../logging/log.hpp"

namespace Spark {
namespace math {

u32 roll_dice(const u32 min, const u32 max) {
    // Check if the range is valid
    if (min > max) {
        SPARK_ERROR("Error: min must be less than or equal to max");
        return -1;
    }

    // Check if min or max is negative
    if (min < 0 || max < 0) {
        SPARK_ERROR("Error: min and max must be non-negative integers");
        return 0; // Return 0 to indicate error
    }

    // Create a random number generator and seed it using random_device
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<u32> dist(min, max);
    
    // Generate a random number within the defined range (min to max)
    return static_cast<u32>(dist(gen));
}

std::vector<u32> multi_dice_roll(const u32 min, const u32 max, const u32 num_dice) {
    std::vector<u32> rolled_dice;

    // Check if the range is valid
    if (min > max) {
         SPARK_ERROR("Error: min must be less than or equal to max");
        return rolled_dice; //Return an empty array
    }

    for (u32 i = 0; i <= num_dice; ++i) {
        rolled_dice.push_back(roll_dice(min, max));
    }

    return rolled_dice;
}

std::vector<u32> roll_unfair_dice(const u32 min, const unsigned int max, const std::vector<f64>& percentages) {
    std::vector<u32> rolled_dice;

    // Check if the range is valid
    if (min > max) {
        SPARK_ERROR("Error: min must be less than or equal to max");
        return rolled_dice;
    }

    // Check if the number of percentages matches the range
    if (percentages.size() != (max - min + 1)) {
        std::cerr << "Error: Number of percentages must match the range of values between min and max" << std::endl;
        return rolled_dice; // Return an empty vector
    }

    f64 sum = 0.0;
    // Check if any percentage is less than 0 and sum then up
    for (f64 percentage : percentages) {
        if (percentage < 0) {
            SPARK_ERROR("Error: Percentages must be non-negative");
            return rolled_dice; // Return an empty vector
        }

        sum += percentage;
    }

    if (abs(sum - 1) > 1e-4) {
        SPARK_ERROR("Error: The sum of percentages must be 1");
        return rolled_dice; // Return an empty vector
    }

    std::random_device rd;
    std::mt19937 gen(rd());

    // Create a discrete distribution based on the given percentages
    std::discrete_distribution<u32> dist(percentages.begin(), percentages.end());

    for (u32 i = 0; i < percentages.size(); ++i) {
        rolled_dice.push_back(dist(gen) + min); // Add min to shift the result to the desired range
    }

    return rolled_dice;
}

CoinFlip flip_coin() {
    // Create a random number generator
    std::random_device rd;
    std::mt19937 gen(rd()); // Mersenne Twister engine
    std::bernoulli_distribution dis(0.5); // 50% chance for true (heads) and 50% chance for false (tails)

    // Flip the coin
    bool result = dis(gen);

    return result ? CoinFlip::HEADS : CoinFlip::TAILS;
}

CoinFlip flip_unfair_coin(const double percentage) {
    // Check if percentage is within the range [0, 1]
    if (percentage < 0 || percentage > 1) {
        std::cerr << "Error: Percentage must be between 0 and 1" << std::endl;
        return;
    }

    // Create a random number generator
    std::random_device rd;
    std::mt19937 gen(rd()); // Mersenne Twister engine
    std::uniform_real_distribution<> dis(0.0, 1.0); // Generate numbers between 0 and 1

    // Flip the coin
    if (dis(gen) < percentage) {
        return CoinFlip::HEADS;
    } else {
        return CoinFlip::TAILS;
    }
}
} // namespace math
} // namespace Spark
