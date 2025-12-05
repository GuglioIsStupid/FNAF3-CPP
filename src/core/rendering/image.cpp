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

    SDL_Surface* surface = IMG_Load((filepath).c_str());
    if (!surface) {
        Common::error("Unable to load image! SDL_image Error: " + std::string(SDL_GetError()));
        return false;
    }

    m_width = surface->w;
    m_height = surface->h;

    SDL_Surface* formattedSurface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_ABGR8888);
    if (!formattedSurface) {
        Common::error("Unable to convert surface to ABGR8888! SDL Error: " + std::string(SDL_GetError()));
        SDL_DestroySurface(surface);
        return false;
    }

    uint8_t* pixels = (uint8_t*)formattedSurface->pixels;

    for (int i = 0; i < m_width * m_height * 4; i += 4) {
        uint8_t& r = pixels[i + 0];
        uint8_t& g = pixels[i + 1];
        uint8_t& b = pixels[i + 2];
        uint8_t& a = pixels[i + 3];

        r = (r * a) / 255;
        g = (g * a) / 255;
        b = (b * a) / 255;
    }

    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0,
                    GL_BGRA, GL_UNSIGNED_BYTE, formattedSurface->pixels);
        
        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
            Common::error("glTexImage2D failed: " + std::to_string(err));
    #else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0,
                    GL_RGBA, GL_UNSIGNED_BYTE, formattedSurface->pixels);
    #endif

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    SDL_DestroySurface(formattedSurface);
    SDL_DestroySurface(surface);

    #if IMAGE_LOGGING
        GLint maxTextures;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextures);
        Common::info("Loaded image: " + filepath + " (" + std::to_string(m_width) + "x" + std::to_string(m_height) + ") | " 
            + std::to_string(m_textureID) + " / " + std::to_string(maxTextures));
    #endif

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

void Image::render(int x, int y, int width, int height, float rotation, int /*originX*/, int /*originY*/) {
    if (!isLoaded()) return;

    int renderWidth  = (width  == -1) ? m_width  : width;
    int renderHeight = (height == -1) ? m_height : height;

    bool flipX = renderWidth < 0;
    bool flipY = renderHeight < 0;

    float absWidth  = static_cast<float>(std::abs(renderWidth));
    float absHeight = static_cast<float>(std::abs(renderHeight));

    float originX = 0.0f;
    float originY = 0.0f;

    switch (anchor) {
        case AnchorMode::TopLeft:
            originX = 0.0f;
            originY = 0.0f;
            break;
        case AnchorMode::Center:
            originX = absWidth * 0.5f;
            originY = absHeight * 0.5f;
            break;
        case AnchorMode::Custom:
            originX = static_cast<float>(hotspotX);
            originY = static_cast<float>(hotspotY);
            break;
    }

    // Define corners relative to origin
    float corners[4][2] = {
        { -originX,       -originY       },
        { absWidth - originX, -originY   },
        { absWidth - originX, absHeight - originY },
        { -originX,       absHeight - originY }
    };

    // Apply rotation
    if (rotation != 0.0f) {
        float rad = rotation * (PI / 180.0f);
        float cosR = std::cos(rad);
        float sinR = std::sin(rad);
        for (int i = 0; i < 4; i++) {
            float px = corners[i][0];
            float py = corners[i][1];
            corners[i][0] = px * cosR - py * sinR;
            corners[i][1] = px * sinR + py * cosR;
        }
    }

    for (int i = 0; i < 4; i++) {
        corners[i][0] += static_cast<float>(x);
        corners[i][1] += static_cast<float>(y);

        int cx = static_cast<int>(corners[i][0]);
        int cy = static_cast<int>(corners[i][1]);
        std::tie(corners[i][0], corners[i][1]) = Common::screenToGLCoords(cx, cy, Common::width, Common::height);
    }

    float u0 = flipX ? 1.0f : 0.0f;
    float u1 = flipX ? 0.0f : 1.0f;
    float v0 = flipY ? 1.0f : 0.0f;
    float v1 = flipY ? 0.0f : 1.0f;

    using namespace Core::Rendering::GL2D;
    float r = m_hasTint ? m_tintR : 1.0f;
    float g = m_hasTint ? m_tintG : 1.0f;
    float b = m_hasTint ? m_tintB : 1.0f;
    float a = m_hasTint ? m_tintA : 1.0f;

    std::vector<Vertex> verts = {
        {corners[0][0], corners[0][1], u0, v0, r,g,b,a},
        {corners[1][0], corners[1][1], u1, v0, r,g,b,a},
        {corners[2][0], corners[2][1], u1, v1, r,g,b,a},

        {corners[2][0], corners[2][1], u1, v1, r,g,b,a},
        {corners[3][0], corners[3][1], u0, v1, r,g,b,a},
        {corners[0][0], corners[0][1], u0, v0, r,g,b,a},
    };

    glEnable(GL_BLEND);
    applyBlendMode(this);

    GL2D::drawTriangles(verts, m_textureID);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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