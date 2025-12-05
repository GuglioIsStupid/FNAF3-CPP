#include "text.hpp"
#include <common/log.hpp>
#include <common/common.hpp>
#include <core/rendering/gl2d.hpp>
#include <core/rendering/colour.hpp>
#include <codecvt>
#include <algorithm>
#include <filesystem> // to check if file exists

#include <core/game.hpp> // for getExecutableDirectory

namespace {

static std::vector<char32_t> utf8_to_codepoints(const std::string& s) {
    std::vector<char32_t> out;
    size_t i = 0;
    while (i < s.size()) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        char32_t cp = 0;
        size_t extra = 0;
        if (c < 0x80) { cp = c; extra = 0; }
        else if ((c & 0xE0) == 0xC0) { cp = c & 0x1F; extra = 1; }
        else if ((c & 0xF0) == 0xE0) { cp = c & 0x0F; extra = 2; }
        else if ((c & 0xF8) == 0xF0) { cp = c & 0x07; extra = 3; }
        else { ++i; continue; }

        if (i + extra >= s.size()) break;
        bool ok = true;
        for (size_t k = 1; k <= extra; ++k) {
            unsigned char cc = static_cast<unsigned char>(s[i + k]);
            if ((cc & 0xC0) != 0x80) { ok = false; break; }
            cp = (cp << 6) | (cc & 0x3F);
        }
        if (ok) {
            out.push_back(cp);
            i += 1 + extra;
        } else {
            ++i;
        }
    }
    return out;
}

}

namespace Core {
namespace Rendering {

TextRenderer::TextRenderer()
    : m_initialized(false), m_ft(nullptr) {
    initialize();
}

TextRenderer::~TextRenderer() {
    cleanup();
}

TextRenderer& TextRenderer::getInstance() {
    static TextRenderer instance;
    return instance;
}

void TextRenderer::initialize() {
    if (m_initialized) return;

    if (FT_Init_FreeType(&m_ft)) {
        Common::error("Failed to initialize FreeType");
        return;
    }

    m_initialized = true;
    Common::info("TextRenderer (FreeType) initialized");
}

bool TextRenderer::loadFont(const std::string& name,
                            const std::string& filepath,
                            int size) {
    if (!m_initialized) initialize();
    if (!m_initialized) return false;

    if (m_fonts.count(name)) {
        unloadFont(name);
    }

    FT_Face face;
    std::string fullPath = Core::Game::getExecutableDirectory() + filepath;
    if (FT_New_Face(m_ft, fullPath.c_str(), 0, &face)) {
        Common::error("Failed to load font: " + fullPath);
        // print if it exists
        if (!std::filesystem::exists(fullPath)) {
            Common::error("Font file does not exist: " + fullPath);
        } else {
            Common::error("Font file exists but failed to load: " + fullPath);
        }
        return false;
    }

    FT_Set_Pixel_Sizes(face, 0, size);

    m_fonts[name] = std::make_unique<FontData>(face, size, filepath);
    if (m_currentFont.empty()) m_currentFont = name;

    std::string vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    std::string renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

    bool isIntegrated = false;

    std::string v = Common::lowercase(vendor);
    std::string r = Common::lowercase(renderer);

    if (v.find("intel") != std::string::npos) // theres dedicated intel gpus but like i literally don't have access to one so i'll just that sdf wont work
        isIntegrated = true;

    if (v.find("amd") != std::string::npos || v.find("advanced micro devices") != std::string::npos || (v.find("ati") != std::string::npos)) {

        if (r.find("radeon(tm)") != std::string::npos ||
            r.find("radeon graphics") != std::string::npos ||
            r.find("mobile") != std::string::npos ||
            r.find("vega") != std::string::npos ||
            r.find("apu") != std::string::npos) {

            isIntegrated = true;
        }

        if (r.find("rx") != std::string::npos &&
            r.find("graphics") == std::string::npos) {
            isIntegrated = false;
        }
    }

    m_useSDF = !isIntegrated;

    Common::info("Loaded font: " + name + " (SDF: " + std::string(m_useSDF ? "enabled" : "disabled") + ")");
    return true;
}

void TextRenderer::setCurrentFont(const std::string& name) {
    if (m_fonts.count(name)) m_currentFont = name;
    else Common::error("Font not found: " + name);
}

void TextRenderer::unloadFont(const std::string& name) {
    auto it = m_fonts.find(name);
    if (it != m_fonts.end()) {
        FT_Done_Face(it->second->face);
        m_fonts.erase(it);
    }
}

void TextRenderer::unloadAllFonts() {
    for (auto& f : m_fonts)
        FT_Done_Face(f.second->face);
    m_fonts.clear();
}

Glyph& TextRenderer::loadGlyph(char32_t ch) {
    if (m_glyphCache.count(ch))
        return m_glyphCache[ch];

    if (!m_fonts.count(m_currentFont)) {
        static Glyph empty;
        return empty;
    }

    FT_Face face = m_fonts[m_currentFont]->face;
    FT_UInt glyph_index = FT_Get_Char_Index(face, static_cast<FT_ULong>(ch));
    if (glyph_index == 0) {
        Common::warn("Glyph not found for codepoint");
        m_glyphCache[ch] = Glyph{};
        return m_glyphCache[ch];
    }

    if (FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT)) {
        Common::warn("Failed to FT_Load_Glyph");
        m_glyphCache[ch] = Glyph{};
        return m_glyphCache[ch];
    }

    FT_GlyphSlot g = face->glyph;

    if (m_useSDF) {
        if (FT_Render_Glyph(g, (FT_Render_Mode)FT_RENDER_MODE_SDF)) {
            if (FT_Render_Glyph(g, FT_RENDER_MODE_NORMAL)) {
                Common::warn("Glyph failed both SDF and NORMAL render");
                m_glyphCache[ch] = Glyph{};
                return m_glyphCache[ch];
            }
        }
    } else {
        if (FT_Render_Glyph(g, FT_RENDER_MODE_NORMAL)) {
            Common::warn("Glyph NORMAL render failed");
            m_glyphCache[ch] = Glyph{};
            return m_glyphCache[ch];
        }
    }

    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_R8,
        g->bitmap.width,
        g->bitmap.rows,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        g->bitmap.buffer
    );

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_RED };
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);

    Glyph glyph;
    glyph.texture = tex;
    glyph.width = g->bitmap.width;
    glyph.height = g->bitmap.rows;
    glyph.bearingX = g->bitmap_left;
    glyph.bearingY = g->bitmap_top;
    glyph.advance  = g->advance.x >> 6;

    m_glyphCache[ch] = glyph;
    return m_glyphCache[ch];
}

void TextRenderer::print(const std::string& text, int x, int y,
                         float rotation, float scaleX, float scaleY,
                         float originX, float originY) {
    if (!m_initialized) return;
    if (text.empty()) return;

    auto cps = utf8_to_codepoints(text);
    renderGlyphString(cps, x, y, rotation, scaleX, scaleY, originX, originY);
}

void TextRenderer::renderGlyphString(const std::vector<char32_t>& codepoints, int x, int y,
                                     float rotation, float scaleX, float scaleY,
                                     float originX, float originY) {
    if (!m_fonts.count(m_currentFont)) return;

    int posX = x;
    int baselineY = y;

    for (char32_t cp : codepoints) {
        Glyph& g = loadGlyph(cp);
        if (!g.texture) continue;

        int xpos = posX + static_cast<int>(g.bearingX * scaleX);
        int ypos = baselineY - static_cast<int>(g.bearingY * scaleY);

        float corners[4][2] = {
            { (float)xpos,                 (float)ypos },
            { (float)(xpos + g.width),     (float)ypos },
            { (float)(xpos + g.width),     (float)(ypos + g.height * scaleY) },
            { (float)xpos,                 (float)(ypos + g.height * scaleY) }
        };

        for (int i = 0; i < 4; i++) {
            float vx = corners[i][0] / (float)m_virtualWidth * 2.f - 1.f;
            float vy = 1.f - (corners[i][1] / (float)m_virtualHeight) * 2.f;
            corners[i][0] = vx;
            corners[i][1] = vy;
        }

        auto col = Core::Rendering::getColor();
        float R = col[0];
        float G = col[1];
        float B = col[2];
        float A = col[3];

        std::vector<GL2D::Vertex> verts = {
            { corners[0][0], corners[0][1], 0, 0, R,G,B,A },
            { corners[1][0], corners[1][1], 1, 0, R,G,B,A },
            { corners[2][0], corners[2][1], 1, 1, R,G,B,A },

            { corners[2][0], corners[2][1], 1, 1, R,G,B,A },
            { corners[3][0], corners[3][1], 0, 1, R,G,B,A },
            { corners[0][0], corners[0][1], 0, 0, R,G,B,A }
        };

        GL2D::drawTriangles(verts, g.texture, m_useSDF);

        posX += static_cast<int>(g.advance * scaleX);
    }
}

void TextRenderer::getTextSize(const std::string& text, int* w, int* h) {
    int width = 0, height = 0;
    auto cps = utf8_to_codepoints(text);

    for (char32_t cp : cps) {
        Glyph& g = loadGlyph(cp);
        width += g.advance;
        height = std::max(height, g.height);
    }

    if (w) *w = width;
    if (h) *h = height;
}

void TextRenderer::cleanup() {
    unloadAllFonts();
    if (m_ft) FT_Done_FreeType(m_ft);
    m_glyphCache.clear();
}

void TextRenderer::setViewport(int x, int y, int w, int h, int windowW, int windowH) {
    m_viewportX = x;
    m_viewportY = y;
    m_viewportW = w;
    m_viewportH = h;
    m_windowW = windowW;
    m_windowH = windowH;
}

bool loadFont(const std::string& name, const std::string& filepath, int size) {
    return TextRenderer::getInstance().loadFont(name, filepath, size);
}

void setFont(const std::string& name) {
    TextRenderer::getInstance().setCurrentFont(name);
}

void print(const std::string& text, int x, int y,
           float rotation, float scaleX, float scaleY,
           float originX, float originY) {
    TextRenderer::getInstance().print(text, x, y,
                                      rotation, scaleX, scaleY,
                                      originX, originY);
}

void getTextSize(const std::string& text, int* width, int* height) {
    TextRenderer::getInstance().getTextSize(text, width, height);
}

} // namespace Rendering
} // namespace Core
