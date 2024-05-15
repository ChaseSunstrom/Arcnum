#ifndef RANDOM_HPP
#define RANDOM_HPP

#include ".\starterIncludeFile.hpp"
#include <random>

unsigned int rollDice(const unsigned int min, const unsigned int max) {
    // Check if the range is valid
    if (min > max) {
        cerr << "Error: min must be less than or equal to max" << endl;
        return -1;
    }

    // Check if min or max is negative
    if (min < 0 || max < 0) {
        cerr << "Error: min and max must be non-negative integers" << endl;
        return 0; // Return 0 to indicate error
    }

    // Create a random number generator and seed it using random_device
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<unsigned int> dist(min, max);
    
    // Generate a random number within the defined range (min to max)
    return static_cast<unsigned int>(dist(gen));
}

vector<unsigned int> multiDiceRoll(const unsigned int min, const unsigned int max, const unsigned int numDice) {
    vector<unsigned int> rolledDice;

    // Check if the range is valid
    if (min > max) {
        cerr << "Error: min must be less than or equal to max" << endl;
        return rolledDice;
    }
    
    for (unsigned int i = 0; i <= numDice; ++i) {
        rolledDice.push_back(rollDice(min, max));
    }

    return rolledDice;
}

vector<unsigned int> rollUnfairDice(const unsigned int min, const unsigned int max, const vector<double>& percentages) {
    vector<unsigned int> rolledDice;

    // Check if the range is valid
    if (min > max) {
        cerr << "Error: min must be less than or equal to max" << endl;
        return rolledDice;
    }

    // Check if the number of percentages matches the range
    if (percentages.size() != (max - min + 1)) {
        cerr << "Error: Number of percentages must match the range of values between min and max" << endl;
        return rolledDice; // Return an empty vector
    }

    double sum = 0.0;
    // Check if any percentage is less than 0 and sum then up
    for (double percentage : percentages) {
        if (percentage < 0) {
            cerr << "Error: Percentages must be non-negative" << endl;
            return rolledDice; // Return an empty vector
        }

        sum += percentage;
    }

    if (abs(sum - 1) > 1e-4) {
        cerr << "Error: The sum of percentages must be 1" << endl;
        return rolledDice; // Return an empty vector
    }

    random_device rd;
    mt19937 gen(rd());

    // Create a discrete distribution based on the given percentages
    discrete_distribution<unsigned int> dist(percentages.begin(), percentages.end());

    for (unsigned int i = 0; i < percentages.size(); ++i) {
        rolledDice.push_back(dist(gen) + min); // Add min to shift the result to the desired range
    }

    return rolledDice;
}

void flipCoin() {
    // Create a random number generator
    random_device rd;
    mt19937 gen(rd()); // Mersenne Twister engine
    bernoulli_distribution dis(0.5); // 50% chance for true (heads) and 50% chance for false (tails)

    // Flip the coin
    bool result = dis(gen);

    if (result) {
        cout << "Heads" << std::endl;
    } else {
        cout << "Tails" << std::endl;
    }
}

void flipUnfairCoin(const double percentage) {
    // Check if percentage is within the range [0, 1]
    if (percentage < 0 || percentage > 1) {
        cerr << "Error: Percentage must be between 0 and 1" << endl;
        return;
    }

    // Create a random number generator
    random_device rd;
    mt19937 gen(rd()); // Mersenne Twister engine
    uniform_real_distribution<> dis(0.0, 1.0); // Generate numbers between 0 and 1

    // Flip the coin
    if (dis(gen) < percentage) {
        cout << "Heads" << std::endl;
    } else {
        cout << "Tails" << std::endl;
    }
}

#endif //RANDOM_HPP