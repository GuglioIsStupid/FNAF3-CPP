#include "game.hpp"

#include <iostream>
#include <filesystem>
#include <string>

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_opengl3.h>

#include <core/viewport.hpp>
#include <core/timer.hpp>
#include <core/input.hpp>

#include <core/rendering/gl2d.hpp>
#include <core/rendering/image.hpp>
#include <core/rendering/text.hpp>

// for printf
#include <stdio.h>

namespace Core {

Game::Game() : m_window(nullptr), m_glContext(nullptr), m_isRunning(false) {}

int Game::init(const char* title, int width, int height, int _windowWidth, int _windowHeight) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 0);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

    #ifdef OS_WINDOWS
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    #endif

    m_window = SDL_CreateWindow(title, 1024, 768, SDL_WINDOW_OPENGL);
    if (!m_window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        return -1;
    }

    SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_SetWindowResizable(m_window, true);

    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext) {
        fprintf(stderr, "SDL_GL_CreateContext failed: %s\n", SDL_GetError());
        return -1;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return -1;
    }

    printf("GL_VERSION = %s\n", glGetString(GL_VERSION));
    printf("GL_VENDOR = %s\n", glGetString(GL_VENDOR));
    printf("GL_RENDERER = %s\n", glGetString(GL_RENDERER));

    glEnable(GL_MULTISAMPLE);

    Core::Rendering::GL2D::init();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForOpenGL(m_window, m_glContext);
    ImGui_ImplOpenGL3_Init("#version 330");

    m_isRunning = true;
    
    gameWidth = width;
    gameHeight = height;

    Common::width = width;
    Common::height = height;

    Core::Rendering::TextRenderer::getInstance().setViewport(
        0, 0, width, height, 
        _windowWidth > 0 ? _windowWidth : width,
        _windowHeight > 0 ? _windowHeight : height
    );

    return 0;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL3_ProcessEvent(&event);
        Core::input.processEvent(event);
        if (event.type == SDL_EVENT_QUIT) {
            m_isRunning = false;
        } else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
            onResize(event.window.data1, event.window.data2);
        }

        if (m_state) m_state->handleEvents(*this, event);
    }
}

void Game::update() {
    Core::Input::get().update(Core::Timer::getDeltaTime());
    if (m_state) {
        m_state->update(*this, Core::Timer::getDeltaTime());
    }
}

void Game::onResize(int w, int h) {
    Viewport vp = calculateViewport(w, h, gameWidth, gameHeight);
    glViewport(vp.x, vp.y, vp.w, vp.h);

    if (m_state) m_state->onResize(*this, w, h);
    
    Core::Rendering::TextRenderer::getInstance().setViewport(
        vp.x, vp.y, vp.w, vp.h, w, h
    );
}

void Game::render() {
    int winW, winH;
    SDL_GetWindowSize(m_window, &winW, &winH);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    Viewport vp = calculateViewport(winW, winH, gameWidth, gameHeight);

    Core::Rendering::TextRenderer::getInstance().setViewport(
        0, 0, winW, winH, winW, winH
    );

    glViewport(0, 0, winW, winH);
    {
        static const Core::Rendering::GL2D::Vertex screenQuad[6] = {
            {-1.f,  1.f, 0.f,0.f, 0.1f,0.1f,0.1f,1.f},
            {-1.f, -1.f, 0.f,0.f, 0.1f,0.1f,0.1f,1.f},
            { 1.f, -1.f, 0.f,0.f, 0.1f,0.1f,0.1f,1.f},

            { 1.f, -1.f, 0.f,0.f, 0.1f,0.1f,0.1f,1.f},
            { 1.f,  1.f, 0.f,0.f, 0.1f,0.1f,0.1f,1.f},
            {-1.f,  1.f, 0.f,0.f, 0.1f,0.1f,0.1f,1.f},
        };
        Core::Rendering::GL2D::drawTriangles(screenQuad, 6);
    }

    Core::Rendering::TextRenderer::getInstance().setViewport(
        vp.x, vp.y, vp.w, vp.h, winW, winH
    );

    glViewport(vp.x, vp.y, vp.w, vp.h);
    {
        static const Core::Rendering::GL2D::Vertex screenQuad[6] = {
            {-1.f,  1.f, 0.f,0.f, 0.f,0.f,0.f,1.f},
            {-1.f, -1.f, 0.f,0.f, 0.f,0.f,0.f,1.f},
            { 1.f, -1.f, 0.f,0.f, 0.f,0.f,0.f,1.f},

            { 1.f, -1.f, 0.f,0.f, 0.f,0.f,0.f,1.f},
            { 1.f,  1.f, 0.f,0.f, 0.f,0.f,0.f,1.f},
            {-1.f,  1.f, 0.f,0.f, 0.f,0.f,0.f,1.f},
        };
        Core::Rendering::GL2D::drawTriangles(screenQuad, 6);
    }

    if (m_state) {
        m_state->render(*this);
    }

    char buf[32];
    std::snprintf(buf, sizeof(buf), "FPS: %d", (int)Core::Timer::getFPS());
    Core::Rendering::print(buf, 10, 20);

    glViewport(0, 0, winW, winH);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(m_window);
}

bool Game::isRunning() const {
    return m_isRunning;
}

void Game::changeState(std::unique_ptr<State> newState) {
    // KILL the old state
    if (m_state) m_state->leave(*this);

    m_state = std::move(newState);
    if (m_state) m_state->enter(*this);

    // flush some events
    SDL_FlushEvent(SDL_EVENT_KEY_DOWN);
    SDL_FlushEvent(SDL_EVENT_KEY_UP);
    SDL_FlushEvent(SDL_EVENT_TEXT_INPUT);
    SDL_FlushEvent(SDL_EVENT_TEXT_EDITING);
    SDL_FlushEvent(SDL_EVENT_MOUSE_MOTION);
    SDL_FlushEvent(SDL_EVENT_MOUSE_BUTTON_DOWN);
    SDL_FlushEvent(SDL_EVENT_MOUSE_BUTTON_UP);
    SDL_FlushEvent(SDL_EVENT_MOUSE_WHEEL);
}

void Game::cleanup() {
    if (m_state) {
        m_state->leave(*this);
        m_state.reset();
    }
}

Game::~Game() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    if (m_glContext) {
        SDL_GL_DestroyContext(m_glContext);
    }
    if (m_window) {
        SDL_DestroyWindow(m_window);
    }
    SDL_Quit();
}

std::string Game::getExecutableDirectory(bool forwardSlash) {
    const char* basePath = SDL_GetBasePath();
    if (basePath) {
        std::string path(basePath);
        if (forwardSlash) {
            std::replace(path.begin(), path.end(), '\\', '/');
        }
        return path;
    }

    return std::filesystem::current_path().string() + "/";
}

std::string Game::getSaveDirectory(bool forwardSlash) {
    std::string appName = "Rit";
    #ifdef DEBUG
        appName += "-Debug";
    #endif
    const char* savePath = SDL_GetPrefPath("CubitosTeam", appName.c_str());
    if (savePath) {
        std::string path(savePath);
        if (forwardSlash) {
            std::replace(path.begin(), path.end(), '\\', '/');
        }
        return path;
    }

    return std::filesystem::current_path().string() + "/";
}

int Game::convertMouseX(int x) const {
    int winW, winH;
    SDL_GetWindowSize(m_window, &winW, &winH);
    Viewport vp = calculateViewport(winW, winH, gameWidth, gameHeight);
    if (x < vp.x || x > vp.x + vp.w) {
        return -1;
    }
    return (x - vp.x) * gameWidth / vp.w;
}

int Game::convertMouseY(int y) const {
    int winW, winH;
    SDL_GetWindowSize(m_window, &winW, &winH);
    Viewport vp = calculateViewport(winW, winH, gameWidth, gameHeight);
    if (y < vp.y || y > vp.y + vp.h) {
        return -1;
    }
    return (y - vp.y) * gameHeight / vp.h;
}

} // namespace Core