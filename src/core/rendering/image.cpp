#include "image.hpp"
#include <common/common.hpp>
#include <common/log.hpp>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL.h>
#include <cmath>
#include <core/rendering/gl2d.hpp>

namespace Core {
namespace Rendering {

#ifdef DEBUG
    #define IMAGE_LOGGING false
#else
    #define IMAGE_LOGGING false
#endif

Image::Image() 
    : m_textureID(0), m_width(0), m_height(0)
    , m_tintR(1.0f), m_tintG(1.0f), m_tintB(1.0f), m_tintA(1.0f)
    , m_hasTint(false) {
}

Image::~Image() {
    cleanup();
}

bool Image::load(const std::string& filepath) {
    cleanup();

    SDL_Surface* surface = IMG_Load(filepath.c_str());
    if (!surface) {
        Common::error("Unable to load image! SDL_image Error: " + std::string(SDL_GetError()));
        return false;
    }

    m_width = surface->w;
    m_height = surface->h;

    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    SDL_DestroySurface(surface);

    return true;
}

void Image::setBlendMode(BlendMode mode) {
    blendMode = mode;
}

void Image::setCustomBlend(GLenum src, GLenum dst) {
    blendMode = BlendMode::Custom;
    srcFactor = src;
    dstFactor = dst;
}

static void applyBlendMode(const Image* img) {
    switch (img->blendMode) {
        case BlendMode::Normal:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;

        case BlendMode::Additive:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            break;

        case BlendMode::Multiply:
            glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
            break;

        case BlendMode::Screen:
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
            break;

        case BlendMode::Custom:
            glBlendFunc(img->srcFactor, img->dstFactor);
            break;
    }
}

void Image::render(int x, int y, int width, int height, float rotation, int originX, int originY) {
    if (!isLoaded()) return;

    float w = (width == -1) ? m_width : (float)width;
    float h = (height == -1) ? m_height : (float)height;

    float ox = 0, oy = 0;
    switch (anchor) {
        case AnchorMode::TopLeft:
            ox = 0;
            oy = 0;
            break;
        case AnchorMode::Center:
            ox = w/2;
            oy = h/2;
            break;
        case AnchorMode::Custom:
            ox = (float)hotspotX;
            oy = (float)hotspotY;
            break;
    }

    float corners[4][2] = {
        { -ox,       -oy    },
        { w - ox,    -oy    },
        { w - ox,    h - oy },
        { -ox,       h - oy }
    };

    if (rotation != 0.0f) {
        float rad = rotation * (3.14159265f / 180.f);
        float cosR = std::cos(rad), sinR = std::sin(rad);
        for (int i=0;i<4;i++){
            float px = corners[i][0], py = corners[i][1];
            corners[i][0] = px*cosR - py*sinR;
            corners[i][1] = px*sinR + py*cosR;
        }
    }

    for (int i=0;i<4;i++) {
        corners[i][0] += x;
        corners[i][1] += y;
        std::tie(corners[i][0], corners[i][1]) = Common::screenToGLCoords(
            (int)corners[i][0], (int)corners[i][1], Common::width, Common::height);
    }

    float u0=0, u1=1, v0=0, v1=1;
    if (w < 0) std::swap(u0,u1);
    if (h < 0) std::swap(v0,v1);

    float r = m_hasTint ? m_tintR : 1.f;
    float g = m_hasTint ? m_tintG : 1.f;
    float b = m_hasTint ? m_tintB : 1.f;
    float a = m_hasTint ? m_tintA : 1.f;

    using namespace Core::Rendering::GL2D;
    Vertex verts[6] = {
        {corners[0][0], corners[0][1], u0, v0, r,g,b,a},
        {corners[1][0], corners[1][1], u1, v0, r,g,b,a},
        {corners[2][0], corners[2][1], u1, v1, r,g,b,a},
        {corners[2][0], corners[2][1], u1, v1, r,g,b,a},
        {corners[3][0], corners[3][1], u0, v1, r,g,b,a},
        {corners[0][0], corners[0][1], u0, v0, r,g,b,a},
    };

    GL2D::drawTriangles(verts, 6, m_textureID);
}

void Image::setWidth(int width) {
    if (width < 0) width = 0;
    m_width = width;
}

void Image::setHeight(int height) {
    if (height < 0) height = 0;
    m_height = height;
}

void Image::setDimensions(int width, int height) {
    setWidth(width);
    setHeight(height);
}

void Image::setTint(float r, float g, float b, float a) {
    m_tintR = r;
    m_tintG = g;
    m_tintB = b;
    m_tintA = a;
    m_hasTint = true;
}

void Image::clearTint() {
    m_hasTint = false;
}

void Image::cleanup() {
    if (m_textureID != 0) {
        glDeleteTextures(1, &m_textureID);
        m_textureID = 0;
    }
    m_width = 0;
    m_height = 0;
}

void Image::unload() {
    cleanup();
}

} // namespace Rendering
} // namespace Core