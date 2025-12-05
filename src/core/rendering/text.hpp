#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace Core {
namespace Rendering {

struct Glyph {
    GLuint texture;
    int width;
    int height;
    int bearingX;
    int bearingY;
    long advance;
};

struct FontData {
    FT_Face face;
    int size;
    std::string filepath;

    FontData(FT_Face f, int s, const std::string& path)
        : face(f), size(s), filepath(path) {}
};

struct CachedText {
    GLuint texture = 0;
    int width = 0;
    int height = 0;
};

class TextRenderer {
public:
    static TextRenderer& getInstance();

    bool loadFont(const std::string& name, const std::string& filepath, int size);
    void setCurrentFont(const std::string& name);
    void unloadFont(const std::string& name);
    void unloadAllFonts();

    void print(const std::string& text, int x, int y,
               float rotation = 0.0f, float scaleX = 1.0f, float scaleY = 1.0f,
               float originX = 0.0f, float originY = 0.0f);

    void getTextSize(const std::string& text, int* width, int* height);
    
    void setViewport(int x, int y, int w, int h, int windowW, int windowH);

    void cleanup();

private:
    TextRenderer();
    ~TextRenderer();

    void initialize();
    void renderGlyphString(const std::vector<char32_t>& codepoints, int x, int y,
               float rotation, float scaleX, float scaleY,
               float originX, float originY);

    Glyph& loadGlyph(char32_t ch);

    std::unordered_map<std::string, std::unique_ptr<FontData>> m_fonts;
    std::unordered_map<char32_t, Glyph> m_glyphCache;

    std::string m_currentFont;
    bool m_initialized;

    int m_virtualWidth  = 1024;
    int m_virtualHeight = 768;

    int m_viewportX = 0;
    int m_viewportY = 0;
    int m_viewportW = 1024;
    int m_viewportH = 768;
    int m_windowW   = 1024;
    int m_windowH   = 768;

    FT_Library m_ft;

    bool m_useSDF = true;
};

bool loadFont(const std::string& name, const std::string& filepath, int size);
void setFont(const std::string& name);
void print(const std::string& text, int x, int y,
           float rotation = 0.0f, float scaleX = 1.0f, float scaleY = 1.0f,
           float originX = 0.0f, float originY = 0.0f);
void getTextSize(const std::string& text, int* width, int* height);

} // namespace Rendering
} // namespace Core
