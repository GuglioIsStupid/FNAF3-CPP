#pragma once
#include <SDL3/SDL.h>
#include <string>

namespace Core {

class Game;

class State {
public:
    bool allowScripting = false;
    std::string state_name = "unnamed";

    virtual ~State() = default;

    virtual void enter([[maybe_unused]] Game& game) {}
    virtual void handleEvents([[maybe_unused]] Game& game, [[maybe_unused]] SDL_Event& event) = 0;
    virtual void update([[maybe_unused]] Game& game, [[maybe_unused]] float dt) = 0;
    virtual void render([[maybe_unused]] Game& game) {}
    virtual void onResize([[maybe_unused]] Game& game, [[maybe_unused]] int width, [[maybe_unused]] int height) {}
    virtual void leave([[maybe_unused]] Game& game) {}
};

} // namespace Core
