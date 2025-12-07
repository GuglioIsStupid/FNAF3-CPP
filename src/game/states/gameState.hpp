#pragma once
#include <core/state.hpp>
#include <game/gameObject.hpp>
#include <game/asset.hpp>

namespace game {
namespace states {

class GameState : public Core::State {
public:
    std::string state_name = "test";
    void enter(Core::Game& game) override;
    void handleEvents(Core::Game& game, SDL_Event& event) override;
    void update(Core::Game& game, float dt) override;
    void render(Core::Game& game) override;
    void leave(Core::Game& game) override;

private:
    // HITBOXES
    Object hitboxLeftSLOW;
    Object hitboxRightSLOW;
    Object hitboxLeftFAST;
    Object hitboxRightFAST;

    // OFFICE
    Object foreground;
    Object fan;
    Object black;
    Object background;
    Object nose;

    bool cameraOpen, systemOpen = false;
    bool balloonBoyScare, foxyScare = false;
    bool ventilationError = false;

    int gx, gy = 0;
    int maxgy = 0;
    int mingy = -976;
    float gxSUB = 0;
    float gySUB = 0;

    std::vector<Object*> animatedObjectStack;
};

} // namespace states
} // namespace game