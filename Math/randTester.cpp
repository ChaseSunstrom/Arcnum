#include "random.hpp"

int main() {
    // Example usage
    unsigned int min = 1;
    unsigned int max = 6;
    vector<double> percentages = {1, 0.21, 0.156, 0.25, 0.04, -0.656}; // Total sum should be 1.0
    vector<unsigned int> results = rollUnfairDice(min, max, percentages);

    cout << "Rolled dice: ";
    for (auto result : results) {
        cout << result << " ";
    }
    cout << endl;

    return 0;
}