#pragma once

#include <array>
#include <common/common.hpp>
#include <tuple>
namespace Core {
namespace Rendering {
namespace Colour {

extern std::array<float, 4> currentColor;

} // namespace Colour

void setColor(float r, float g, float b, float a = 1.0f);
std::array<float, 4> getColor();

// hsvToRgb and rgbToHsv functions based on code from:
// https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both

std::tuple<float,float,float> hsvToRgb(float h, float s, float v);
std::tuple<float,float,float> rgbToHsv(float r, float g, float b);

} // namespace Rendering
} // namespace Core