#pragma once

#include "asset.hpp"
#include <core/rendering/image.hpp>

class Asset; class Animation;

class Object {
public:
    int x, y;
    float parallax;
    int width, height;
    float alpha;
    int reverse;
    float animationSpeed;
    float animationSpeedSecondary;
    bool clickable;
    bool isInvisible;
    bool mouseReleased;
    bool floating;
    bool forceShow;
    Asset *currentAsset;
    Animation *currentAnimation;
    Animation *currentSecondaryAnimation;
    Object();
    Object(Asset *asset, int x, int y, int *gx, int *gy);
    Object(int x, int y, int *gx, int *gy, int width, int height);
    void render();
    void setVisibility(bool visible);
    int getPosition(bool axis);
    bool isMouseHovering();
    bool isMouseClicking();

    Core::Rendering::BlendMode blendMode = ::Core::Rendering::BlendMode::Normal;
    GLenum srcFactor;
    GLenum dstFactor;

private:
    int *gx, *gy;
};
