#include "random.hpp"
#include <cmath>

namespace Core {
namespace Helpers {

bool Random::s_initialized = false;

std::mt19937& Random::getGenerator() {
    static std::mt19937 generator;
    if (!s_initialized) {
        std::random_device rd;
        generator.seed(rd());
        s_initialized = true;
    }
    return generator;
}

void Random::init(unsigned int seed) {
    if (seed == 0) {
        std::random_device rd;
        seed = rd();
    }
    getGenerator().seed(seed);
    s_initialized = true;
}

int Random::randInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(getGenerator());
}

float Random::randFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(getGenerator());
}

double Random::randDouble(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(getGenerator());
}

bool Random::randBool(float probability) {
    std::bernoulli_distribution dist(probability);
    return dist(getGenerator());
}

} // namespace Helpers
} // namespace Core
