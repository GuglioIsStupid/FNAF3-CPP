#include <iostream>

#include <SDL3/SDL.h>

#include <common/common.hpp>

#include <core/game.hpp>
#include <core/timer.hpp>
#include <core/input.hpp>

#include <core/rendering/text.hpp>

#include <game/states/titleState.hpp>

#include <game/asset.hpp>

int main(int argc, char** argv) {
    Core::Game& game = Core::Game::getInstance();

    char titleBuffer[256];
    snprintf(titleBuffer, sizeof(titleBuffer), "%s (%s-%s)", "Five Nights at Freddy's 3", 
        Common::getOSName().c_str(),
        Common::getBuildType().c_str()
    );

    if (game.init(titleBuffer, 1024, 768) != 0) {
        std::cerr << "Failed to initialize game." << std::endl;
        return -1;
    }

    Assets::initAudio();
    Assets::loadAllAssets();
    game.changeState<game::states::TitleState>();

    auto& input = Core::Input::get();
    input.addAction("confirm");
    input.bindKey("confirm", SDL_SCANCODE_RETURN);

    input.addAction("mouse_down");
    input.bindMouseButton("mouse_down", SDL_BUTTON_LEFT);

    Core::Rendering::loadFont("default", "assets/fonts/DejaVuLGCSansMono.ttf", 16);

    while (game.isRunning()) {
        Core::Timer::tick();
        Assets::updateAudio();
        game.handleEvents();
        game.update();
        game.render();
    }

    game.cleanup();

    return 0;
}
