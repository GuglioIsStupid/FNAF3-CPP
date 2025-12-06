#pragma once
#include <core/state.hpp>
#include <game/gameObject.hpp>
#include <game/asset.hpp>

namespace game {
namespace states {

class NightState : public Core::State {
public:
    std::string state_name = "night";
    void enter(Core::Game& game) override;
    void handleEvents(Core::Game& game, SDL_Event& event) override;
    void update(Core::Game& game, float dt) override;
    void render(Core::Game& game) override;
    void leave(Core::Game& game) override;

private:
    Object blipTop;
    Object blipBottom;
    Object nightText;

    float timer = 0.0f;
    float fade = 0.0f;

    int gx, gy = 0;
};

} // namespace states
} // namespace game