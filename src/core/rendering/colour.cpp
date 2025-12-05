#include "colour.hpp"
#include <common/common.hpp>
#include <array>
#include <glad/glad.h>
#include <tuple>

namespace Core {
namespace Rendering {
namespace Colour {
std::array<float, 4> currentColor = {1.0f, 1.0f, 1.0f, 1.0f};
} // namespace Colour

void setColor(float r, float g, float b, float a) {
    Colour::currentColor = {r, g, b, a};
}

std::array<float, 4> getColor() {
    return Colour::currentColor;
}

// hsvToRgb and rgbToHsv functions based on code from:
// https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both

std::tuple<float,float,float> hsvToRgb(float h, float s, float v) {
    float r, g, b;

    int i = static_cast<int>(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    switch (i % 6) {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
        default: r = g = b = 0;
    }

    return {r, g, b};
}

std::tuple<float,float,float> rgbToHsv(float r, float g, float b) {
    float max = Common::max(r, Common::max(g, b));
    float min = Common::min(r, Common::min(g, b));
    float h, s, v = max;

    float d = max - min;
    s = (max == 0) ? 0 : d / max;

    if (max == min) {
        h = 0;
    } else {
        if (max == r) {
            h = (g - b) / d + (g < b ? 6 : 0);
        } else if (max == g) {
            h = (b - r) / d + 2;
        } else {
            h = (r - g) / d + 4;
        }
        h /= 6;
    }

    return {h, s, v};
}

} // namespace Rendering
} // namespace Core