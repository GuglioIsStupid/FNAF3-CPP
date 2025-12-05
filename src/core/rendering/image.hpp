#pragma once

#include <string>
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <vector>

namespace Core {
namespace Rendering {

enum class BlendMode {
    Normal,
    Additive,
    Multiply,
    Screen,
    Custom
};

enum class AnchorMode {
    TopLeft,
    Center,
    Custom
};

class Image {
public:
    Image();
    ~Image();

    bool load(const std::string& filepath);
    
    void render(int x = 0, int y = 0, int width = -1, int height = -1, float rotation = 0.0f, int originX = 0, int originY = 0);
    
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    void setWidth(int width);
    void setHeight(int height);
    void setDimensions(int width, int height);
    bool isLoaded() const { return m_textureID != 0; }
    
    void setTint(float r, float g, float b, float a = 1.0f);
    void clearTint();

    void unload();

    BlendMode blendMode = BlendMode::Normal;
    GLenum srcFactor;
    GLenum dstFactor;
    void setBlendMode(BlendMode mode);
    void setCustomBlend(GLenum src, GLenum dst);

    AnchorMode anchor = AnchorMode::TopLeft;
    int hotspotX = 0;
    int hotspotY = 0;

    void setAnchorTopLeft() { anchor = AnchorMode::TopLeft; }
    void setAnchorCenter() { anchor = AnchorMode::Center; }
    void setAnchorCustom(int x, int y) {
        anchor = AnchorMode::Custom;
        hotspotX = x;
        hotspotY = y;
    }

private:
    GLuint m_textureID;
    int m_width;
    int m_height;
    float m_tintR, m_tintG, m_tintB, m_tintA;
    bool m_hasTint;
    
    
    void cleanup();
};

} // namespace Rendering
} // namespace Core