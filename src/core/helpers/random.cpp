#include "random.hpp"
#include <random>
#include <algorithm>

namespace Core {
namespace Helpers {

std::mt19937& rng() {
    static std::mt19937 generator([]{
        std::random_device rd;
        return std::mt19937(rd());
    }());
    return generator;
}

void initRandom(unsigned int seed) {
    if (seed == 0) {
        std::random_device rd;
        seed = rd();
    }
    rng().seed(seed);
}

int randInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng());
}

float randFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng());
}

double randDouble(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(rng());
}

bool randBool(float probability) {
    std::bernoulli_distribution dist(probability);
    return dist(rng());
}

} // namespace Helpers
} // namespace Core
