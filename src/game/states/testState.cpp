#include "testState.hpp"

#include <common/common.hpp>
#include <core/timer.hpp>

namespace game {
namespace states {

void TestState::enter(Core::Game& /* game */) {
    testObject = Object(
        new Asset("assets/images/204.png"),
        0, 0,
        &gx, &gy
    );
}

void TestState::handleEvents(Core::Game& /* game */, SDL_Event& /* event */) {

}

void TestState::update(Core::Game& /* game */, float dt) {

}

void TestState::render(Core::Game& game) {
    testObject.render();
}

void TestState::leave(Core::Game& /* game */) {
}

} // namespace states
} // namespace game