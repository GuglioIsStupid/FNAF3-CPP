#include "nightState.hpp"

#include <common/common.hpp>
#include <core/timer.hpp>

#include <game/data.hpp>

#include <core/rendering/shapes.hpp>
#include <core/rendering/colour.hpp>

#include <core/game.hpp>

namespace game {
namespace states {

void NightState::enter(Core::Game& /* game */) {
    Assets::stopAudio("titlemusic");
    Assets::playSound("startday", 0);
    nightText = Object(Assets::assetList[0], 512, 374, &gx, &gy);
    switch(Data::night) {
        case 1:
            nightText.currentAsset = Assets::assetList[1098];
            break;
        case 2:
            nightText.currentAsset = Assets::assetList[837];
            break;
        case 3:
            nightText.currentAsset = Assets::assetList[838];
            break;
        case 4:
            nightText.currentAsset = Assets::assetList[842];
            break;
        case 5:
            nightText.currentAsset = Assets::assetList[843];
            break;
        case 6:
            nightText.currentAsset = Assets::assetList[844];
            break;
        case 7:
            nightText.currentAsset = Assets::assetList[430];
            break;
    }
    nightText.currentAsset->setAnchorCenter();
    nightText.width = 201;
    nightText.height = 104;
    nightText.currentAsset->setDimensions(nightText.width, nightText.height);

    blipTop = Object(Assets::assetList[1126], 0, 334, &gx, &gy);
    blipBottom = Object(Assets::assetList[1126], 0, 406, &gx, &gy);
    blipTop.currentAsset->setAnchorTopLeft();
    blipBottom.currentAsset->setAnchorTopLeft();

    blipTop.width = 1024;
    blipTop.height = 30;
    blipBottom.width = 1024;
    blipBottom.height = 30;

    Animation blipAnim = Animation();
    blipAnim.add(Assets::assetList[1126]);
    blipAnim.add(Assets::assetList[1128]);
    blipAnim.add(Assets::assetList[1123]);
    blipAnim.add(Assets::assetList[1120]);
    blipAnim.add(Assets::assetList[1121]);
    blipAnim.add(Assets::assetList[1119]);
    blipAnim.add(Assets::assetList[1121]);
    blipAnim.add(Assets::assetList[1125]);
    blipAnim.add(Assets::assetList[1121]);
    blipAnim.add(Assets::assetList[1121]);
    blipAnim.add(Assets::assetList[1125]);
    blipAnim.add(Assets::assetList[1125]);
    blipAnim.add(Assets::assetList[1151]);

    blipTop.currentAnimation = new Animation(blipAnim);
    blipBottom.currentAnimation = new Animation(blipAnim);

    std::cout << "Entering NightState\n";
}

void NightState::handleEvents(Core::Game& /* game */, SDL_Event& /* event */) {

}

void NightState::update(Core::Game& /* game */, float dt) {
    if (blipTop.currentAnimation->isPlaying) {
        blipTop.currentAnimation->currentSubframe += 50.f * dt;
        blipTop.currentAnimation->currentFrame = static_cast<int>(blipTop.currentAnimation->currentSubframe);
        if (blipTop.currentAnimation->currentFrame >= blipTop.currentAnimation->frames.size()) {
            blipTop.currentAnimation->currentSubframe = 0.0f;
            blipTop.currentAnimation->isPlaying = false;
            blipTop.currentAnimation->currentFrame = blipTop.currentAnimation->frames.size() - 1;
        }
        blipTop.currentAsset = blipTop.currentAnimation->frames[blipTop.currentAnimation->currentFrame];
        blipBottom.currentAnimation->currentSubframe += 50.f * dt;
        blipBottom.currentAnimation->currentFrame = static_cast<int>(blipBottom.currentAnimation->currentSubframe);
        if (blipBottom.currentAnimation->currentFrame >= blipBottom.currentAnimation->frames.size()) {
            blipBottom.currentAnimation->currentSubframe = 0.0f;
            blipBottom.currentAnimation->isPlaying = false;
            blipBottom.currentAnimation->currentFrame = blipBottom.currentAnimation->frames.size() - 1;
        }
        blipBottom.currentAsset = blipBottom.currentAnimation->frames[blipBottom.currentAnimation->currentFrame];

        blipTop.currentAsset->setDimensions(blipTop.width, blipTop.height);
        blipBottom.currentAsset->setDimensions(blipBottom.width, blipBottom.height);
    }

    if (timer < 3.0f) {
        timer += dt;
    } else {
        fade += dt;
    }
}

void NightState::render(Core::Game& game) {
    nightText.render();
    blipTop.render();
    blipBottom.render();

    Core::Rendering::setColor(0.0f, 0.0f, 0.0f, fade);
    Core::Rendering::Shapes::rectangle(true, 0, 0, 1024, 768);
    Core::Rendering::setColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void NightState::leave(Core::Game& /* game */) {
}

} // namespace states
} // namespace game