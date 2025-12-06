#pragma once
#include <random>
#include <algorithm>

namespace Core {
namespace Helpers {

void initRandom(unsigned int seed = 0);
int randInt(int min, int max);
float randFloat(float min = 0.0f, float max = 1.0f);
double randDouble(double min = 0.0, double max = 1.0);
bool randBool(float probability = 0.5f);

std::mt19937& rng();

} // namespace Helpers
} // namespace Core
