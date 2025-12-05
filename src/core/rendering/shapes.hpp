#pragma once

#include <vector>
#include <tuple>
#include <glad/glad.h>
#include <common/common.hpp>
#include "colour.hpp"
#include <core/rendering/gl2d.hpp>

namespace Core {
namespace Rendering {
namespace Shapes {
void rectangle(bool filled, int x, int y, int width, int height);
void roundedRectangle(bool filled, int x, int y, int width, int height, int radius);
void circle(bool filled, int centerX, int centerY, int radius, int segments = 0);
void line(int x1, int y1, int x2, int y2, float thickness);
void polygon(bool filled, int* vertices, int vertexCount = 0);

void renderThrobber(int centerX, int centerY, int radius, int numSegments, float thickness, float angleOffset);

// Gradient stuff
template<typename... Colors>
void gradientRectangleHoriz(bool filled, int x, int y, int width, int height, Colors... args) {
    std::vector<float> colors = { static_cast<float>(args)... };
    size_t stops = colors.size() / 4;
    if (stops < 2) return;

    auto currentColor = Core::Rendering::getColor();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float glX, glY;
    std::tie(glX, glY) = Common::screenToGLCoords(x, y, Common::width, Common::height);
    float glWidth = (static_cast<float>(width) / Common::width) * 2.0f;
    float glHeight = (static_cast<float>(height) / Common::height) * 2.0f;

    using namespace Core::Rendering::GL2D;
    if (filled) {
        for (size_t i = 0; i < stops - 1; ++i) {
            float t = static_cast<float>(i) / (stops - 1);
            float tNext = static_cast<float>(i + 1) / (stops - 1);
            float xStart = glX + t * glWidth;
            float xEnd = glX + tNext * glWidth;
            float r1 = colors[i*4 + 0], g1 = colors[i*4 + 1], b1 = colors[i*4 + 2], a1 = colors[i*4 + 3];
            float r2 = colors[(i+1)*4 + 0], g2 = colors[(i+1)*4 + 1], b2 = colors[(i+1)*4 + 2], a2 = colors[(i+1)*4 + 3];
            std::vector<Vertex> v = {
                {xStart, glY - glHeight, 0,0, r1,g1,b1,a1},
                {xStart, glY,            0,0, r1,g1,b1,a1},
                {xEnd,   glY,            0,0, r2,g2,b2,a2},

                {xEnd,   glY,            0,0, r2,g2,b2,a2},
                {xEnd,   glY - glHeight, 0,0, r2,g2,b2,a2},
                {xStart, glY - glHeight, 0,0, r1,g1,b1,a1},
            };
            drawTriangles(v);
        }
    } else {
        auto col = currentColor;
        std::vector<Vertex> v = {
            {glX, glY, 0,0, col[0],col[1],col[2],col[3]},
            {glX + glWidth, glY, 0,0, col[0],col[1],col[2],col[3]},
            {glX + glWidth, glY - glHeight, 0,0, col[0],col[1],col[2],col[3]},
            {glX, glY - glHeight, 0,0, col[0],col[1],col[2],col[3]},
        };
        draw(GL_LINE_LOOP, v.data(), v.size());
    }
}

template<typename... Colors>
void gradientRectangleVert(bool filled, int x, int y, int width, int height, Colors... args) {
    std::vector<float> colors = { static_cast<float>(args)... };
    size_t stops = colors.size() / 4;
    if (stops < 2) return;

    auto currentColor = Core::Rendering::getColor();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float glX, glY;
    std::tie(glX, glY) = Common::screenToGLCoords(x, y, Common::width, Common::height);
    float glWidth = (static_cast<float>(width) / Common::width) * 2.0f;
    float glHeight = (static_cast<float>(height) / Common::height) * 2.0f;

    using namespace Core::Rendering::GL2D;
    if (filled) {
        for (size_t i = 0; i < stops - 1; ++i) {
            float t = static_cast<float>(i) / (stops - 1);
            float tNext = static_cast<float>(i + 1) / (stops - 1);
            float yStart = glY - t * glHeight;
            float yEnd = glY - tNext * glHeight;
            float r1 = colors[i*4 + 0], g1 = colors[i*4 + 1], b1 = colors[i*4 + 2], a1 = colors[i*4 + 3];
            float r2 = colors[(i+1)*4 + 0], g2 = colors[(i+1)*4 + 1], b2 = colors[(i+1)*4 + 2], a2 = colors[(i+1)*4 + 3];
            std::vector<Vertex> v = {
                {glX, yEnd,   0,0, r1,g1,b1,a1},
                {glX, yStart, 0,0, r1,g1,b1,a1},
                {glX + glWidth, yStart, 0,0, r2,g2,b2,a2},

                {glX + glWidth, yStart, 0,0, r2,g2,b2,a2},
                {glX + glWidth, yEnd,   0,0, r2,g2,b2,a2},
                {glX, yEnd,   0,0, r1,g1,b1,a1},
            };
            drawTriangles(v);
        }
    } else {
        auto col = currentColor;
        std::vector<Vertex> v = {
            {glX, glY, 0,0, col[0],col[1],col[2],col[3]},
            {glX + glWidth, glY, 0,0, col[0],col[1],col[2],col[3]},
            {glX + glWidth, glY - glHeight, 0,0, col[0],col[1],col[2],col[3]},
            {glX, glY - glHeight, 0,0, col[0],col[1],col[2],col[3]},
        };
        draw(GL_LINE_LOOP, v.data(), v.size());
    }
}

} // namespace Shapes
} // namespace Rendering
} // namespace Core