#pragma once

#include <common/common.hpp>
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <memory>

#include <core/state.hpp>

namespace Core {

class Game {
public:
    Game();
    ~Game();

    static Game& getInstance() {
        static Game instance;
        return instance;
    }

    int init(const char* title, int width, int height, int _windowWidth = -1, int _windowHeight = -1);
    void handleEvents();
    void update();
    void onResize(int w, int h);
    void render();
    bool isRunning() const;
    void cleanup();

    void changeState(std::unique_ptr<State> newState);
    template<typename T, typename... Args>
    void changeState(Args&&... args) {
        static_assert(std::is_base_of<State, T>::value, "T must derive from State");
        changeState(std::make_unique<T>(std::forward<Args>(args)...));
    }

    SDL_Window* getWindow() const { return m_window; }
    std::string getLastError() const { return m_lastError; }
    void setLastError(const std::string& error) { m_lastError = error; }

    int gameWidth, gameHeight;

    static std::string getExecutableDirectory(bool forwardSlash = true);
    static std::string getSaveDirectory(bool forwardSlash = true);

    int convertMouseX(int x) const;
    int convertMouseY(int y) const;

private:
    bool m_isRunning;

    SDL_Window* m_window;
    SDL_GLContext m_glContext;

    std::string m_lastError;

    std::unique_ptr<State> m_state;
};

} // namespace Core