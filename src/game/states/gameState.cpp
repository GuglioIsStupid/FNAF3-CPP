#include "gameState.hpp"

#include <common/common.hpp>
#include <core/timer.hpp>

#include <game/data.hpp>
#include <core/input.hpp>
#include <core/game.hpp>

#define AABB(x1, y1, w1, h1, x2, y2, w2, h2) \
    (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2)

namespace game {
namespace states {

void GameState::enter(Core::Game& /* game */) {
    // HITBOXES
    hitboxRightSLOW = Object(845-(int)362/2, 403-(int)822/2, &gx, &gy, 362, 822);
    hitboxLeftSLOW = Object(171-(int)362/2, 405-(int)822/2, &gx, &gy, 362, 822);
    hitboxRightFAST = Object(894-(int)128/2, 405-(int)822/2, &gx, &gy, 276, 822);
    hitboxLeftFAST = Object(54-(int)276/2, 405-(int)822/2, &gx, &gy, 276, 822);
    hitboxRightSLOW.floating = true;
    hitboxRightSLOW.forceShow = true;
    hitboxLeftSLOW.floating = true;
    hitboxLeftSLOW.forceShow = true;
    hitboxRightFAST.floating = true;
    hitboxLeftFAST.forceShow = true;
    hitboxLeftFAST.floating = true;
    hitboxRightFAST.forceShow = true;

    // OFFICE
    foreground = Object(Assets::assetList[203], 0, 0, &gx, &gy);
    foreground.currentAnimation = new Animation();
    foreground.currentAnimation->add(Assets::assetList[203]);
    foreground.currentAnimation->add(Assets::assetList[204]);
    animatedObjectStack.push_back(&foreground);
    background = Object(Assets::assetList[205], 0, 0, &gx, &gy);
    black = Object(Assets::assetList[1150], 0, 0, &gx, &gy);

    fan = Object(Assets::assetList[18], 1193, 280, &gx, &gy);
    fan.currentAnimation = new Animation();
    for(int i = 0; i < 3; i++){
        fan.currentAnimation->add(Assets::assetList[18 + i]);
    }
    animatedObjectStack.push_back(&fan);
    fan.animationSpeed = 99.f*6.f;

    nose = Object(665, 269, &gx, &gy, 13, 13);
    nose.forceShow = true;
}

void GameState::handleEvents(Core::Game& /* game */, SDL_Event& /* event */) {

}

void GameState::update(Core::Game& g, float dt) {
    fan.currentAnimation->isPlaying = true;
    for (auto& obj : animatedObjectStack) {
        if (obj->currentAnimation && !obj->currentAnimation->frames.empty()) {
            auto& anim = obj->currentAnimation;

            if (anim->isPlaying) {
                anim->currentSubframe += 0.05f * dt * obj->animationSpeed;
                anim->currentFrame = (int)anim->currentSubframe;

                if (anim->currentFrame >= anim->frames.size()) {
                    anim->currentFrame = 0;
                    anim->currentSubframe = 0.0f;
                    anim->isPlaying = false;
                }
            }

            obj->currentAsset = anim->frames[anim->currentFrame];

            obj->currentAsset->blendMode = obj->blendMode;
            obj->currentAsset->srcFactor = obj->srcFactor;
            obj->currentAsset->dstFactor = obj->dstFactor;

            if (obj->reverseExpansion) {
                obj->x += obj->height - obj->currentAsset->height;
                obj->y += obj->width - obj->currentAsset->width;
            }

            obj->width = obj->currentAsset->width;
            obj->height = obj->currentAsset->height;
        }

        if (obj->currentSecondaryAnimation && !obj->currentSecondaryAnimation->frames.empty()) {
            auto& anim = obj->currentSecondaryAnimation;

            if (anim->isPlaying) {
                anim->currentSubframe += 0.05f * dt * obj->animationSpeedSecondary;
                anim->currentFrame = (int)anim->currentSubframe;

                if (anim->currentFrame >= anim->frames.size()) {
                    anim->currentFrame = 0;
                    anim->currentSubframe = 0.0f;
                    anim->isPlaying = false;
                }
            }

            obj->currentAsset = anim->frames[anim->currentFrame];

            obj->currentAsset->blendMode = obj->blendMode;
            obj->currentAsset->srcFactor = obj->srcFactor;
            obj->currentAsset->dstFactor = obj->dstFactor;

            if (obj->reverseExpansion) {
                obj->x += obj->height - obj->currentAsset->height;
                obj->y += obj->width - obj->currentAsset->width;
            }

            obj->width = obj->currentAsset->width;
            obj->height = obj->currentAsset->height;
        }
    }

    if (!cameraOpen && !systemOpen && !balloonBoyScare && !foxyScare) {
        if (hitboxRightFAST.isMouseHovering() && gy - 1 > mingy) gySUB -= 500 * dt;
        if (hitboxLeftFAST.isMouseHovering() && gy + 1 < maxgy) gySUB += 500 * dt;
        if (hitboxRightSLOW.isMouseHovering() && gy - 1 > mingy) gySUB -= 300 * dt;
        if (hitboxLeftSLOW.isMouseHovering() && gy + 1 < maxgy) gySUB += 300 * dt;

        int mouseX = Core::Input::get().getMouseX();
        int mouseY = Core::Input::get().getMouseY();
        mouseX = g.convertMouseX(mouseX);
        mouseY = g.convertMouseY(mouseY);
        if (AABB(mouseX, mouseY, 1, 1, nose.getPosition(0), nose.getPosition(1), nose.width, nose.height) && Core::Input::get().justPressed("mouse_down")) {
            Assets::playSound("PartyFavorraspyPart_AC01__3", 0);
        }
    }

    gx = static_cast<int>(gxSUB);
    gy = static_cast<int>(gySUB);
    //std::cout << "GX: " << gx << " GY: " << gy << "\n";

    if (ventilationError) {

    } else {
        foreground.currentAnimation->isPlaying = false;
        foreground.currentAsset = Assets::assetList[203];
    }
}

void GameState::render(Core::Game& game) {
    background.render();

    foreground.render();
    nose.render();
    fan.render();

    hitboxRightSLOW.render();
    hitboxLeftSLOW.render();
    hitboxRightFAST.render();
    hitboxLeftFAST.render();

    //black.render();
}

void GameState::leave(Core::Game& /* game */) {
}

} // namespace states
} // namespace game