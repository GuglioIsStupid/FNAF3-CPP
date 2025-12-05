#pragma once
#include "basic.hpp"
#include <core/rendering/image.hpp>
#include <common/log.hpp>
#include <common/common.hpp>

namespace Core {
namespace Objects {

class Sprite : public Basic {
public:
    Core::Rendering::Image image;
    float color[3] = {1.0f, 1.0f, 1.0f};
    float alpha = 1.0f;
    float rotation = 0.0f;
    int originX = 0;
    int originY = 0;
    bool visible = true;
    int x = 0;
    int y = 0; 
    int width = -1;
    int height = -1;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    bool onlyRenderIfVisible = true;

    Sprite() {}

    Sprite(const std::string& imagePath) {
        setImage(imagePath);
    }

    void setImage(const std::string& imagePath) {
        if (!image.load(imagePath)) {
            Common::warn("Failed to load image: " + imagePath);
        }
        if (width == -1) width = image.getWidth();
        if (height == -1) height = image.getHeight();
    }

    void render() override {
        if (!visible) return;
        if (onlyRenderIfVisible && (x + width * scaleX < 0 || x > Common::width || y + height * scaleY < 0 || y > Common::height))
            return;
        image.setTint(color[0], color[1], color[2], alpha);
        image.render(x, y, static_cast<int>(width * scaleX), static_cast<int>(height * scaleY), rotation, originX, originY);
        image.clearTint();
    }

    void setColor(float r = 1.0f, float g = 1.0f, float b = 1.0f) {
        color[0] = r;
        color[1] = g;
        color[2] = b;
    }

    void setTint(float r, float g, float b, float a) {
        setColor(r, g, b);
        alpha = a;
    }

    void clearTint() {
        setColor(1.0f, 1.0f, 1.0f);
        alpha = 1.0f;
    }

    int getWidth() const {
        return (width == -1) ? image.getWidth() : width;
    }

    int getHeight() const {
        return (height == -1) ? image.getHeight() : height;
    }
};

} // namespace Objects
} // namespace Core