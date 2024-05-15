#include "random.hpp"

namespace Spark {
namespace math {
//i64 is int
//u32 is unsigned int
//f64 is double

u32 roll_dice(const u32 min, const u32 max) {
    // Check if the range is valid
    if (min > max) {
        std::cerr << "Error: min must be less than or equal to max" << std::endl;
        return -1;
    }

    // Check if min or max is negative
    if (min < 0 || max < 0) {
        std::cerr << "Error: min and max must be non-negative integers" << std::endl;
        return 0; // Return 0 to indicate error
    }

    // Create a random number generator and seed it using random_device
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned int> dist(min, max);
    
    // Generate a random number within the defined range (min to max)
    return static_cast<u32>(dist(gen));
}

std::vector<u32> multi_dice_roll(const u32 min, const u32 max, const u32 num_dice) {
    std::vector<u32> rolled_dice;

    // Check if the range is valid
    if (min > max) {
        std::cerr << "Error: min must be less than or equal to max" << std::endl;
        return rolled_dice; //Return an empty array
    }

    for (u32 i = 0; i <= num_dice; ++i) {
        rolled_dice.push_back(roll_dice(min, max));
    }

    return rolled_dice;
}

std::vector<u32> roll_unfair_dice(const u32 min, const unsigned int max, const std::vector<f64>& percentages) {
    std::vector<u32> rolledDice;

    // Check if the range is valid
    if (min > max) {
        std::cerr << "Error: min must be less than or equal to max" << std::endl;
        return rolledDice;
    }

    // Check if the number of percentages matches the range
    if (percentages.size() != (max - min + 1)) {
        std::cerr << "Error: Number of percentages must match the range of values between min and max" << std::endl;
        return rolledDice; // Return an empty vector
    }

    double sum = 0.0;
    // Check if any percentage is less than 0 and sum then up
    for (double percentage : percentages) {
        if (percentage < 0) {
            std::cerr << "Error: Percentages must be non-negative" << std::endl;
            return rolledDice; // Return an empty vector
        }

        sum += percentage;
    }

    if (abs(sum - 1) > 1e-4) {
        std::cerr << "Error: The sum of percentages must be 1" << std::endl;
        return rolledDice; // Return an empty vector
    }

    std::random_device rd;
    std::mt19937 gen(rd());

    // Create a discrete distribution based on the given percentages
    std::discrete_distribution<u32> dist(percentages.begin(), percentages.end());

    for (unsigned int i = 0; i < percentages.size(); ++i) {
        rolledDice.push_back(dist(gen) + min); // Add min to shift the result to the desired range
    }

    return rolledDice;
}

void flip_coin() {
    // Create a random number generator
    std::random_device rd;
    std::mt19937 gen(rd()); // Mersenne Twister engine
    std::bernoulli_distribution dis(0.5); // 50% chance for true (heads) and 50% chance for false (tails)

    // Flip the coin
    bool result = dis(gen);

    if (result) {
        std::cout << "Heads" << std::endl;
    } else {
        std::cout << "Tails" << std::endl;
    }
}

void flip_unfair_coin(const double percentage) {
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
        std::cout << "Heads" << std::endl;
    } else {
        std::cout << "Tails" << std::endl;
    }
}
} // namespace math
} // namespace Spark