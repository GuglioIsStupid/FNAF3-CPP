#pragma once

#include <glad/glad.h>
#include <vector>

namespace Core {
namespace Rendering {
namespace GL2D {

struct Vertex {
    float x, y;
    float u, v;
    float r, g, b, a;
};

bool init();
void shutdown();

void draw(GLenum mode, const Vertex* verts, size_t count, GLuint texture = 0, bool sdf = false);
inline void draw(GLenum mode, const std::vector<Vertex>& verts, GLuint texture = 0, bool sdf = false) {
    if (!verts.empty()) draw(mode, verts.data(), verts.size(), texture, sdf);
}

void drawTriangles(const Vertex* verts, size_t count, GLuint texture = 0, bool sdf = false);
void drawTriangles(const std::vector<Vertex>& verts, GLuint texture = 0, bool sdf = false);

} // namespace GL2D
} // namespace Rendering
} // namespace Core
