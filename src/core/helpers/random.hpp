#pragma once

#include <random>
#include <algorithm>

namespace Core {
namespace Helpers {

class Random {
public:
    static void init(unsigned int seed = 0);
    static int randInt(int min, int max);
    static float randFloat(float min = 0.0f, float max = 1.0f);
    static double randDouble(double min = 0.0, double max = 1.0);
    static bool randBool(float probability = 0.5f);

    template<typename Iterator>
    static Iterator choice(Iterator begin, Iterator end) {
        if (begin == end) return end;
        auto dist = std::distance(begin, end);
        std::uniform_int_distribution<decltype(dist)> distribution(0, dist - 1);
        std::advance(begin, distribution(getGenerator()));
        return begin;
    }

    template<typename Iterator>
    static void shuffle(Iterator begin, Iterator end) {
        std::shuffle(begin, end, getGenerator());
    }

private:
    static std::mt19937& getGenerator();
    static bool s_initialized;
};

} // namespace Helpers
} // namespace Core
