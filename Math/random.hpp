#ifndef RANDOM_HPP
#define RANDOM_HPP

#include ".\starterIncludeFile.hpp"
#include <random>

unsigned int rollDice(const unsigned int min, const unsigned int max) {
    // Create a random number generator and seed it using random_device
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<unsigned int> dist(min, max);
    
    // Generate a random number within the defined range (min to max)
    return static_cast<unsigned int>(dist(gen));
}

vector<unsigned int> multiDiceRoll(const unsigned int min, const unsigned int max, const unsigned int numDice) {
    vector<unsigned int> rolledDice;
    
    for (unsigned int i = 0; i <= numDice; ++i) {
        rolledDice.push_back(rollDice(min, max));
    }

    return rolledDice;
}

#endif //RANDOM_HPP