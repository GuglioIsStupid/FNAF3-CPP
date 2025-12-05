#include "shapes.hpp"
#include <common/common.hpp>

#include <glad/glad.h>
#include <cmath>
#include <vector>
#include <core/rendering/gl2d.hpp>

namespace Core {
namespace Rendering {
namespace Shapes {
void rectangle(bool filled, int x, int y, int width, int height) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float glX, glY;
    std::tie(glX, glY) = Common::screenToGLCoords(x, y, Common::width, Common::height);

    float glWidth = (static_cast<float>(width) / Common::width) * 2.0f;
    float glHeight = (static_cast<float>(height) / Common::height) * 2.0f;
    
    using namespace Core::Rendering::GL2D;
    auto col = Core::Rendering::getColor();
    if (filled) {
        std::vector<Vertex> v = {
            {glX, glY, 0,0, col[0],col[1],col[2],col[3]},
            {glX + glWidth, glY, 0,0, col[0],col[1],col[2],col[3]},
            {glX + glWidth, glY - glHeight, 0,0, col[0],col[1],col[2],col[3]},

            {glX + glWidth, glY - glHeight, 0,0, col[0],col[1],col[2],col[3]},
            {glX, glY - glHeight, 0,0, col[0],col[1],col[2],col[3]},
            {glX, glY, 0,0, col[0],col[1],col[2],col[3]},
        };
        GL2D::drawTriangles(v);
    } else {
        std::vector<Vertex> v = {
            {glX, glY, 0,0, col[0],col[1],col[2],col[3]},
            {glX + glWidth, glY, 0,0, col[0],col[1],col[2],col[3]},

            {glX + glWidth, glY, 0,0, col[0],col[1],col[2],col[3]},
            {glX + glWidth, glY - glHeight, 0,0, col[0],col[1],col[2],col[3]},

            {glX + glWidth, glY - glHeight, 0,0, col[0],col[1],col[2],col[3]},
            {glX, glY - glHeight, 0,0, col[0],col[1],col[2],col[3]},

            {glX, glY - glHeight, 0,0, col[0],col[1],col[2],col[3]},
            {glX, glY, 0,0, col[0],col[1],col[2],col[3]},
        };
        GL2D::draw(GL_LINES, v.data(), v.size());
    }
}

void roundedRectangle(bool filled, int x, int y, int width, int height, int radius) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (radius <= 0) {
        rectangle(filled, x, y, width, height);
        return;
    }

    if (radius * 2 > width) radius = width / 2;
    if (radius * 2 > height) radius = height / 2;

    float glX, glY;
    std::tie(glX, glY) = Common::screenToGLCoords(x, y, Common::width, Common::height);

    float glWidth = (static_cast<float>(width) / Common::width) * 2.0f;
    float glHeight = (static_cast<float>(height) / Common::height) * 2.0f;
    float glRadiusX = (static_cast<float>(radius) / Common::width) * 2.0f;
    float glRadiusY = (static_cast<float>(radius) / Common::height) * 2.0f;

    int segments = 16;

    if (filled) {
        rectangle(true, x + radius, y - radius, width - 2*radius, height - 2*radius);

        rectangle(true, x + radius, y, width - 2*radius, radius);
        rectangle(true, x + radius, y - height + radius, width - 2*radius, radius);
        rectangle(true, x, y - radius, radius, height - 2*radius);
        rectangle(true, x + width - radius, y - radius, radius, height - 2*radius);

        auto drawCorner = [&](float cx, float cy, float startAngle) {
            std::vector<GL2D::Vertex> fan;
            auto col = Core::Rendering::getColor();
            fan.push_back({cx, cy, 0,0, col[0],col[1],col[2],col[3]});
            for (int i = 0; i <= segments; ++i) {
                float angle = startAngle + i * (3.14159265f / 2.0f) / segments;
                fan.push_back({cx + cos(angle) * glRadiusX, cy + sin(angle) * glRadiusY, 0,0, col[0],col[1],col[2],col[3]});
            }
            GL2D::drawTriangles(fan);
        };

        drawCorner(glX + glWidth - glRadiusX, glY - glRadiusY, 0.0f);
        drawCorner(glX + glRadiusX, glY - glRadiusY, 3.14159265f / 2.0f);
        drawCorner(glX + glRadiusX, glY - glHeight + glRadiusY, 3.14159265f);
        drawCorner(glX + glWidth - glRadiusX, glY - glHeight + glRadiusY, 3.0f * 3.14159265f / 2.0f);
    } else {
        auto col = Core::Rendering::getColor();
        std::vector<GL2D::Vertex> v;
        auto pushArc = [&](float startAngle, float cx, float cy, bool first) {
            for (int i = 0; i <= segments; ++i) {
                float angle = startAngle + i * (3.14159265f / 2.0f) / segments;
                GL2D::Vertex pt{cx + cos(angle) * glRadiusX, cy + sin(angle) * glRadiusY, 0,0, col[0],col[1],col[2],col[3]};
                if (!first && i == 0 && !v.empty()) {
                }
                v.push_back(pt);
            }
        };
        pushArc(3.14159265f / 2.0f, glX + glRadiusX, glY - glRadiusY, true);
        pushArc(3.14159265f, glX + glRadiusX, glY - glHeight + glRadiusY, false);
        pushArc(3.0f * 3.14159265f / 2.0f, glX + glWidth - glRadiusX, glY - glHeight + glRadiusY, false);
        pushArc(0.0f, glX + glWidth - glRadiusX, glY - glRadiusY, false);
        GL2D::draw(GL_LINE_STRIP, v.data(), v.size());
    }
}

void circle(bool filled, int centerX, int centerY, int radius, int segments) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (segments <= 0) segments = 32;
    if (segments < 3) segments = 3;

    float glCenterX, glCenterY;
    std::tie(glCenterX, glCenterY) = Common::screenToGLCoords(centerX, centerY, Common::width, Common::height);

    float glRadiusX = (radius / static_cast<float>(Common::width)) * 2.0f;
    float glRadiusY = (radius / static_cast<float>(Common::height)) * 2.0f;

    auto col = Core::Rendering::getColor();
    if (filled) {
        std::vector<GL2D::Vertex> v;
        v.push_back({glCenterX, glCenterY, 0,0, col[0],col[1],col[2],col[3]});
        for (int i = 0; i <= segments; ++i) {
            float angle = static_cast<float>(i * 2.0f * PI / segments);
            float x = glCenterX + cos(angle) * glRadiusX;
            float y = glCenterY + sin(angle) * glRadiusY;
            v.push_back({x, y, 0,0, col[0],col[1],col[2],col[3]});
        }
        GL2D::drawTriangles(v);
    } else {
        std::vector<GL2D::Vertex> v;
        for (int i = 0; i < segments; ++i) {
            float angle = static_cast<float>(i * 2.0f * PI / segments);
            float x = glCenterX + cos(angle) * glRadiusX;
            float y = glCenterY + sin(angle) * glRadiusY;
            v.push_back({x, y, 0,0, col[0],col[1],col[2],col[3]});
        }
        GL2D::draw(GL_LINE_LOOP, v.data(), v.size());
    }
}


void line(int x1, int y1, int x2, int y2, float thickness) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float glX1, glY1, glX2, glY2;
    std::tie(glX1, glY1) = Common::screenToGLCoords(x1, y1, Common::width, Common::height);
    std::tie(glX2, glY2) = Common::screenToGLCoords(x2, y2, Common::width, Common::height);

    if (thickness <= 1.0f) {
        auto col = Core::Rendering::getColor();
        GL2D::Vertex v[2] = {
            {glX1, glY1, 0,0, col[0],col[1],col[2],col[3]},
            {glX2, glY2, 0,0, col[0],col[1],col[2],col[3]}
        };
        GL2D::draw(GL_LINES, v, 2);
    } else {
        float glThicknessX = (thickness / Common::width) * 2.0f;
        float glThicknessY = (thickness / Common::height) * 2.0f;

        float angle = atan2(glY2 - glY1, glX2 - glX1);
        float dx = glThicknessX / 2.0f * sin(angle);
        float dy = glThicknessY / 2.0f * -cos(angle);

        auto col = Core::Rendering::getColor();
        std::vector<GL2D::Vertex> v = {
            {glX1 - dx, glY1 - dy, 0,0, col[0],col[1],col[2],col[3]},
            {glX1 + dx, glY1 + dy, 0,0, col[0],col[1],col[2],col[3]},
            {glX2 + dx, glY2 + dy, 0,0, col[0],col[1],col[2],col[3]},

            {glX2 + dx, glY2 + dy, 0,0, col[0],col[1],col[2],col[3]},
            {glX2 - dx, glY2 - dy, 0,0, col[0],col[1],col[2],col[3]},
            {glX1 - dx, glY1 - dy, 0,0, col[0],col[1],col[2],col[3]},
        };
        GL2D::drawTriangles(v);
    }
}

void polygon(bool filled, int* vertices, int vertexCount) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (vertexCount == 0) {
        while (vertices[vertexCount * 2] != 0 || vertices[vertexCount * 2 + 1] != 0) {
            vertexCount++;
        }
    }

    std::vector<GLfloat> glVertices(vertexCount * 2);

    for (int i = 0; i < vertexCount; ++i) {
        float glX, glY;
        std::tie(glX, glY) = Common::screenToGLCoords(vertices[i * 2], vertices[i * 2 + 1], Common::width, Common::height);
        glVertices[i * 2] = glX;
        glVertices[i * 2 + 1] = glY;
    }

    auto col = Core::Rendering::getColor();
    std::vector<GL2D::Vertex> verts;
    verts.reserve(vertexCount);
    for (int i = 0; i < vertexCount; ++i) {
        verts.push_back({glVertices[i * 2], glVertices[i * 2 + 1], 0,0, col[0],col[1],col[2],col[3]});
    }
    if (filled) {
        GL2D::drawTriangles(verts);
    } else {
        GL2D::draw(GL_LINE_LOOP, verts.data(), verts.size());
    }
}

void renderThrobber(int centerX, int centerY, int radius, int numSegments, float /*thickness*/, float angleOffset = 0.0f) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (numSegments <= 0) numSegments = 12;

    float offset = angleOffset;
    for (int i = 0; i < numSegments; ++i) {
        float t = static_cast<float>(i) / numSegments;
        float angle = t * 2.0f * 3.14159265f + offset;

        float tx = centerX + cos(angle) * radius;
        float ty = centerY + sin(angle) * radius;

        setColor(0.7f, 0.7f, 1.0f, t);

        circle(true, static_cast<int>(tx), static_cast<int>(ty), 6, 12);
    }
}


} // namespace Shapes
} // namespace Rendering
} // namespace Core