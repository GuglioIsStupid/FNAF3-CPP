#pragma once
#include <core/state.hpp>
#include <game/gameObject.hpp>
#include <game/asset.hpp>

namespace game {
namespace states {

class TestState : public Core::State {
public:
    std::string state_name = "title";
    void enter(Core::Game& game) override;
    void handleEvents(Core::Game& game, SDL_Event& event) override;
    void update(Core::Game& game, float dt) override;
    void render(Core::Game& game) override;
    void leave(Core::Game& game) override;

private:
    Object testObject;

    int gx, gy = 0;
};

} // namespace states
} // namespace game