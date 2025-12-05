#include "gameObject.hpp"

#include <core/rendering/shapes.hpp>
#include <core/rendering/colour.hpp>

#include <core/input.hpp>
#include <core/game.hpp>

#include <iostream>

Object::Object() {
    gx = 0;
    gy = 0;
    x = 0;
    y = 0;
    parallax = 1.0f;
    width = 0;
    height = 0;
    alpha = 1.0f;
    reverse = 0;
    animationSpeed = 1.0f;
    animationSpeedSecondary = 1.0f;
    clickable = false;
    isInvisible = false;
    mouseReleased = true;
    floating = false;
    forceShow = false;
    currentAsset = nullptr;
    currentAnimation = nullptr;
    currentSecondaryAnimation = nullptr;
}

Object::Object(Asset *asset, int y, int x, int *gx, int *gy) {
    this->gx = gx;
    this->gy = gy;
    this->x = x;
    this->y = y;
    parallax = 1.0f;
    width = asset->getWidth();
    height = asset->getHeight();
    alpha = 1.0f;
    reverse = 0;
    animationSpeed = 1.0f;
    animationSpeedSecondary = 1.0f;
    clickable = false;
    isInvisible = false;
    mouseReleased = true;
    floating = false;
    forceShow = false;
    currentAsset = asset;
    currentAnimation = nullptr;
    currentSecondaryAnimation = nullptr;
}

Object::Object(int y, int x, int *gx, int *gy, int width, int height) {
    this->gx = gx;
    this->gy = gy;
    this->x = x;
    this->y = y;
    parallax = 1.0f;
    this->width = width;
    this->height = height;
    alpha = 1.0f;
    reverse = 0;
    animationSpeed = 1.0f;
    animationSpeedSecondary = 1.0f;
    clickable = false;
    isInvisible = false;
    mouseReleased = true;
    floating = false;
    forceShow = false;
    currentAsset = nullptr;
    currentAnimation = nullptr;
    currentSecondaryAnimation = nullptr;
}

void Object::render() {
    if (forceShow) {
        Core::Rendering::setColor(1.0f, 0.0f, 1.0f, 1.0f);
        Core::Rendering::Shapes::rectangle(false, getPosition(0), getPosition(1), width, height);
        Core::Rendering::setColor(1.0f, 1.0f, 1.0f, 1.0f);
    } else {
        if (isInvisible) return;
        if (currentAsset) {
            currentAsset->setTint(1.0f, 1.0f, 1.0f, alpha);
            currentAsset->render(getPosition(0), getPosition(1), width, height);
        }
    }
}

int Object::getPosition(bool axis) {
    if (axis) {
        if (floating) {
            return x;
        } else {
            return x + static_cast<int>(*gx * parallax);
        }
    } else {
        if (floating) {
            return y;
        } else {
            return y + static_cast<int>(*gy * parallax);
        }
    }
}

void Object::setVisibility(bool invisible){
    isInvisible = invisible;
}

bool Object::isMouseHovering() {
    int mouseX = Core::Input::get().getMouseX();
    int mouseY = Core::Input::get().getMouseY();
    mouseX = Core::Game::getInstance().convertMouseX(mouseX);
    mouseY = Core::Game::getInstance().convertMouseY(mouseY);
    int objX = getPosition(0);
    int objY = getPosition(1);
    return (mouseX >= objX && mouseX <= objX + width &&
            mouseY >= objY && mouseY <= objY + height);
}

bool Object::isMouseClicking() {
    bool hovering = isMouseHovering();
    bool clicking = Core::Input::get().isDown("mouse_left");
    if (hovering && clicking && mouseReleased) {
        mouseReleased = false;
        return true;
    }
    if (!clicking) {
        mouseReleased = true;
    }
    return false;
}