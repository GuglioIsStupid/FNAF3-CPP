#pragma once

#include <core/state.hpp>
#include <game/gameObject.hpp>
#include <game/asset.hpp>
#include <core/helpers/random.hpp>
#include <vector>

namespace game {
namespace states {

class TitleState : public Core::State {
public:
    std::string state_name = "title";
    void enter(Core::Game& game) override;
    void handleEvents(Core::Game& game, SDL_Event& event) override;
    void update(Core::Game& game, float dt) override;
    void render(Core::Game& game) override;
    void leave(Core::Game& game) override;

private:
    Object screenStatic;
    Object lineLeft1;
    Object lineLeft2;
    Object lineLeft3;
    Object lineLeft4;
    Object lineLeft5;
    Object lineLeft6;
    Object lineLeft7;
    Object lineLeft8;
    Object lineLeft9;

    Object lineRight1;
    Object lineRight2;

    Object title;
    Object newGame;
    Object loadGame;
    Object version;
    Object copyright;
    Object holdDel;
    Object selector;
    Object theTrap;
    
    Object freddyFuckingNewspaper;

    int gx, gy = 0;

    float staticAlpha = 1.0f;
    float theTrapAlpha = 1.0f;

    float checksTimer = 0.0f;
    float staticTimer = 0.0f;
    float selectorTimer = 0.0f;
    float theTrapTimer1 = 0.0f;
    float theTrapTimer2 = 0.0f;
    float theTrapRando60fpsTimer = 0.0f;
    float newspaperTimer = 0.0f;
    float frameTimerFor60FPS = 60.0f / 1000.0f;
    int theTrapA, theTrapB = 0;
    int checks = 0;

    std::vector<Object*> animatedObjectStack;

    int state = 0;
};

} // namespace states
} // namespace game