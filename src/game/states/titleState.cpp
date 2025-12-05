#include "TitleState.hpp"

#include <core/objects/sprite.hpp>
#include <core/objects/text.hpp>
#include <common/common.hpp>
#include <core/timer.hpp>
#include <core/input.hpp>
#include <core/game.hpp>
#include <core/rendering/shapes.hpp>
#include <core/rendering/colour.hpp>

#define AABB(x1, y1, w1, h1, x2, y2, w2, h2) \
    (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2)

namespace game {
namespace states {

void TitleState::enter(Core::Game& /* game */) {
    random.init();
    screenStatic = Object(
        Assets::assetList[34],
        0, 0,
        &gx, &gy
    );
    screenStatic.blendMode = Core::Rendering::BlendMode::Custom;
    screenStatic.srcFactor = GL_SRC_ALPHA;
    screenStatic.dstFactor = GL_ONE_MINUS_SRC_ALPHA;
    screenStatic.currentAnimation = new Animation();
    screenStatic.currentAnimation->add(Assets::assetList[33]);
    screenStatic.currentAnimation->add(Assets::assetList[34]);
    screenStatic.currentAnimation->add(Assets::assetList[35]);
    screenStatic.currentAnimation->add(Assets::assetList[36]);
    screenStatic.currentAnimation->add(Assets::assetList[37]);
    screenStatic.animationSpeed = 99.f*10.f;
    animatedObjectStack.push_back(&screenStatic);
    staticAlpha = ((random.randFloat(0, 3) * 25.0f) / 200.0f);

    auto makeLineAnim = [&](int startOffset) {
        Animation* anim = new Animation();

        anim->add(Assets::assetList[1024]);
        anim->add(Assets::assetList[1025]);
        anim->add(Assets::assetList[1028]);
        anim->add(Assets::assetList[1029]);
        anim->add(Assets::assetList[1030]);
        anim->add(Assets::assetList[1031]);
        anim->add(Assets::assetList[1032]);
        anim->add(Assets::assetList[1033]);
        anim->add(Assets::assetList[1034]);
        anim->add(Assets::assetList[1035]);
        anim->add(Assets::assetList[1036]);
        anim->add(Assets::assetList[1037]);
        anim->add(Assets::assetList[1038]);
        anim->add(Assets::assetList[1039]);
        anim->add(Assets::assetList[1040]);
        anim->add(Assets::assetList[1041]);
        anim->add(Assets::assetList[1042]);
        anim->add(Assets::assetList[1043]);

        anim->currentFrame = startOffset % anim->frames.size();
        anim->currentSubframe = anim->currentFrame; 
        anim->isPlaying = true;

        return anim;
    };
    
    float lineLeftAnimSpeed = 24.f*5.25f;
    lineLeft1 = Object(Assets::assetList[1025], 0, 79, &gx, &gy);
    lineLeft2 = Object(Assets::assetList[1024], 0, 127, &gx, &gy);
    lineLeft3 = Object(Assets::assetList[1024], 0, 173, &gx, &gy);
    lineLeft4 = Object(Assets::assetList[1024], 0, 217, &gx, &gy);
    lineLeft5 = Object(Assets::assetList[1024], 0, 261, &gx, &gy);
    lineLeft6 = Object(Assets::assetList[1024], 0, 450, &gx, &gy);
    lineLeft7 = Object(Assets::assetList[1024], 0, 523, &gx, &gy);
    lineLeft8 = Object(Assets::assetList[1024], 0, 592, &gx, &gy);
    lineLeft9 = Object(Assets::assetList[1024], 0, 661, &gx, &gy);
    lineLeft1.currentAnimation = makeLineAnim(0);
    lineLeft1.animationSpeed = lineLeftAnimSpeed;
    lineLeft2.currentAnimation = makeLineAnim(2);
    lineLeft2.animationSpeed = lineLeftAnimSpeed;
    lineLeft3.currentAnimation = makeLineAnim(4);
    lineLeft3.animationSpeed = lineLeftAnimSpeed;
    lineLeft4.currentAnimation = makeLineAnim(6);
    lineLeft4.animationSpeed = lineLeftAnimSpeed;
    lineLeft5.currentAnimation = makeLineAnim(8);
    lineLeft5.animationSpeed = lineLeftAnimSpeed;
    lineLeft6.currentAnimation = makeLineAnim(10);
    lineLeft6.animationSpeed = lineLeftAnimSpeed;
    lineLeft7.currentAnimation = makeLineAnim(12);
    lineLeft7.animationSpeed = lineLeftAnimSpeed;
    lineLeft8.currentAnimation = makeLineAnim(14);
    lineLeft8.animationSpeed = lineLeftAnimSpeed;
    lineLeft9.currentAnimation = makeLineAnim(16);
    lineLeft9.animationSpeed = lineLeftAnimSpeed;

    title = Object(Assets::assetList[155], 118, 172, &gx, &gy);
    title.currentAsset->setAnchorCenter();
    newGame = Object(Assets::assetList[592], 97, 452, &gx, &gy);
    loadGame = Object(Assets::assetList[301], 97, 524, &gx, &gy);
    version = Object(Assets::assetList[160], 916, 706, &gx, &gy);
    copyright = Object(Assets::assetList[849], 924, 741, &gx, &gy);
    copyright.currentAsset->setAnchorCenter();
    holdDel = Object(Assets::assetList[1021], 376, 734, &gx, &gy);
    selector = Object(Assets::assetList[833], 54, 525, &gx, &gy);

    theTrap = Object(Assets::assetList[855], 0, 0, &gx, &gy);
    theTrap.blendMode = Core::Rendering::BlendMode::Custom;
    theTrap.srcFactor = GL_SRC_ALPHA;
    theTrap.dstFactor = GL_ONE_MINUS_SRC_ALPHA;

    freddyFuckingNewspaper = Object(Assets::assetList[0], 0, 0, &gx, &gy);
    freddyFuckingNewspaper.alpha = 0.0f;

    ::Assets::playSound("titlemusic", -1);
}

void TitleState::handleEvents(Core::Game& /* game */, SDL_Event& /* event */) {

}

void TitleState::update(Core::Game& g, float dt) {
    if (state == 0) {
        for (auto& obj : animatedObjectStack) {
            if (obj->currentAnimation && !obj->currentAnimation->frames.empty()) {
                auto& anim = obj->currentAnimation;
                if (anim->isPlaying) {
                    anim->currentSubframe += 0.05f * dt * obj->animationSpeed;
                    anim->currentFrame = static_cast<int>(anim->currentSubframe);
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
            }

            if (obj->currentSecondaryAnimation && !obj->currentSecondaryAnimation->frames.empty()) {
                auto& anim = obj->currentSecondaryAnimation;
                if (anim->isPlaying) {
                    anim->currentSubframe += 0.05f * dt * obj->animationSpeedSecondary;
                    anim->currentFrame = static_cast<int>(anim->currentSubframe);
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
            }
        }
        auto updateLine = [&](Object& obj, float dt) {
            auto& anim = obj.currentAnimation;
            anim->currentSubframe += 0.05f * dt * obj.animationSpeed;
            anim->currentFrame = (int)anim->currentSubframe;
            if (anim->currentFrame >= anim->frames.size()) {
                anim->currentFrame = 0;
                anim->currentSubframe = 0.0f;
            }
            obj.currentAsset = anim->frames[anim->currentFrame];
        };
        updateLine(lineLeft1, dt);
        updateLine(lineLeft2, dt);
        updateLine(lineLeft3, dt);
        updateLine(lineLeft4, dt);
        updateLine(lineLeft5, dt);
        updateLine(lineLeft6, dt);
        updateLine(lineLeft7, dt);
        updateLine(lineLeft8, dt);
        updateLine(lineLeft9, dt);
        checksTimer += dt;
        staticTimer += dt;
        selectorTimer += dt;
        theTrapTimer1 += dt;
        theTrapTimer2 += dt;

        if (checksTimer >= 1.0f) {
            checksTimer = 0.0f;
            checks++;
        }

        if (staticTimer >= 0.04f) {
            staticTimer = 0.0f;
            staticAlpha = (random.randInt(15, 50) / 255.0f);
        }

        if (selectorTimer >= 0.3f) {
            selectorTimer = 0.0f;
        selector.isInvisible = !selector.isInvisible;
        }

        if (theTrapTimer1 >= 1.0f) {
            theTrapTimer1 = 0.0f;
            theTrapB = random.randInt(0, 4);
        }

        float dt60 = dt * 60.0f;
        theTrapRando60fpsTimer += dt60;
        if (theTrapRando60fpsTimer >= frameTimerFor60FPS) {
            if (theTrapB == 1)
                theTrapA = random.randInt(0, 4);
            else
                theTrapA = 5;
            switch (theTrapA) {
                case 0:
                    theTrap.currentAsset = Assets::assetList[855];
                    break;
                case 1:
                    theTrap.currentAsset = Assets::assetList[859];
                    break;
                case 2:
                    theTrap.currentAsset = Assets::assetList[861];
                    break;
                case 3:
                    theTrap.currentAsset = Assets::assetList[864];
                    break;
                default:
                    theTrap.currentAsset = Assets::assetList[862];
                    break;
            }
        }
        if (theTrapTimer2 >= 0.3f) {
            theTrapTimer2 = 0.0f;
            theTrapAlpha = random.randFloat(0.9f, 1.0f);
        }

        screenStatic.currentAnimation->isPlaying = true;
        screenStatic.alpha = staticAlpha;

        theTrap.currentAsset->blendMode = theTrap.blendMode;
        theTrap.currentAsset->srcFactor = theTrap.srcFactor;
        theTrap.currentAsset->dstFactor = theTrap.dstFactor;

        int mx = Core::Input::get().getMouseX();
        int my = Core::Input::get().getMouseY();
        mx = g.convertMouseX(mx);
        my = g.convertMouseY(my);

        if (AABB(mx, my, 1, 1, newGame.getPosition(0), newGame.getPosition(1), newGame.width, newGame.height)) {
            if (selector.x != newGame.x)
                ::Assets::playSound("select", 0);
            selector.x = newGame.x;
        } else if (AABB(mx, my, 1, 1, loadGame.getPosition(0), loadGame.getPosition(1), loadGame.width, loadGame.height)) {
            if (selector.x != loadGame.x)
                ::Assets::playSound("select", 0);
            selector.x = loadGame.x;
        }

        if (Core::Input::get().justPressed("mouse_down")) {
            if (AABB(mx, my, 1, 1, newGame.getPosition(0), newGame.getPosition(1), newGame.width, newGame.height)) {
                ::Assets::playSound("confirm", 0);
                state = 1;
            } else if (AABB(mx, my, 1, 1, loadGame.getPosition(0), loadGame.getPosition(1), loadGame.width, loadGame.height)) {
                ::Assets::playSound("confirm", 0);
                state = 1;
            }
        }
    } else if (state == 1) {
        freddyFuckingNewspaper.alpha += 0.5f * dt;
        newspaperTimer += dt;
        if (newspaperTimer >= 3.0f) {
            newspaperTimer = 0.0f;
            state = 2;
        }
    } else if (state == 2) {
        freddyFuckingNewspaper.alpha -= 0.5f * dt;
        newspaperTimer += dt;
        if (newspaperTimer >= 3.0f) {
            newspaperTimer = 0.0f;
            state = 3; // TODO: Make this change the state
        }
    }
}

void TitleState::render(Core::Game& game) {
    Core::Rendering::setColor(0.0f, 0.0f, 0.0f, 1.0f);
    Core::Rendering::Shapes::rectangle(false, 0, 0, 1024, 768);
    Core::Rendering::setColor(1.0f, 1.0f, 1.0f, 1.0f);

    if (state < 2) {
        theTrap.render();

        lineLeft1.render();
        lineLeft2.render();
        lineLeft3.render();
        lineLeft4.render();
        lineLeft5.render();
        lineLeft6.render();
        lineLeft7.render();
        lineLeft8.render();
        lineLeft9.render();

        title.render();
        newGame.render();
        loadGame.render();
        version.render();
        copyright.render();
        holdDel.render();
        selector.render();

        screenStatic.render();
    }

    freddyFuckingNewspaper.render();
}

void TitleState::leave(Core::Game& /* game */) {
}

} // namespace states
} // namespace game