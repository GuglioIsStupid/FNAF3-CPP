#include "core/input.hpp"
#include <iostream>

using namespace Core;

Input& Core::input = Core::Input::get();

Input& Input::get() {
    static Input instance;
    return instance;
}

Input::~Input() {
    for (auto* gp : gamepads_)
        SDL_CloseGamepad(gp);
}

void Input::openGamepad(int deviceIndex) {
    SDL_Gamepad* pad = SDL_OpenGamepad(deviceIndex);
    if (pad)
        gamepads_.push_back(pad);
}

void Input::closeGamepad(SDL_JoystickID id) {
    gamepads_.erase(std::remove_if(gamepads_.begin(), gamepads_.end(),
        [id](SDL_Gamepad* g) {
            if (SDL_GetJoystickID(SDL_GetGamepadJoystick(g)) == id) {
                SDL_CloseGamepad(g);
                return true;
            }
            return false;
        }), gamepads_.end());
}

void Input::addAction(const std::string& name) {
    if (!actions_.count(name))
        actions_[name] = Action{};
}

void Input::bindKey(const std::string& action, SDL_Scancode scancode) {
    actions_[action].bindings.push_back(ButtonBinding{scancode});
}

void Input::bindButton(const std::string& action, SDL_GamepadButton button) {
    actions_[action].bindings.push_back(ButtonBinding{button});
}

void Input::bindAxis(const std::string& action, SDL_GamepadAxis axis, float threshold) {
    actions_[action].bindings.push_back(ButtonBinding{ButtonBinding::Axis{axis, threshold}});
}

void Input::bindMouseButton(const std::string& action, Uint8 button) {
    actions_[action].bindings.push_back(ButtonBinding{ button });
}

void Input::setRepeat(const std::string& action, bool enabled, float delay, float interval) {
    auto& st = actions_[action].state;
    st.repeatEnabled = enabled;
    st.repeatDelay = delay;
    st.repeatInterval = interval;
}

void Input::processEvent(const SDL_Event& e) {
    switch (e.type) {
        case SDL_EVENT_GAMEPAD_ADDED:
            openGamepad(e.gdevice.which);
            break;
        case SDL_EVENT_GAMEPAD_REMOVED:
            closeGamepad(e.gdevice.which);
            break;
        case SDL_EVENT_MOUSE_MOTION:
            mouseDeltaX_ += e.motion.xrel;
            mouseDeltaY_ += e.motion.yrel;
            mouseX_ = e.motion.x;
            mouseY_ = e.motion.y;
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            mouseButtons_[e.button.button] = true;
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            mouseButtons_[e.button.button] = false;
            break;
        default:
            break;
    }
}

void Input::update(float dt) {
    const bool* keys = SDL_GetKeyboardState(nullptr);

    for (int i = 0; i < 8; i++)
        mouseButtonsPrev_[i] = mouseButtons_[i];

    mouseDeltaX_ = 0;
    mouseDeltaY_ = 0;

    for (auto& [_, action] : actions_) {
        action.state.prevDown = action.state.down;
        action.state.down = false;

        for (auto& bind : action.bindings) {
            if (std::holds_alternative<SDL_Scancode>(bind.key)) {
                if (keys[std::get<SDL_Scancode>(bind.key)]) {
                    action.state.down = true;
                }
            } else if (std::holds_alternative<SDL_GamepadButton>(bind.key)) {
                for (auto* gp : gamepads_) {
                    if (SDL_GetGamepadButton(gp, std::get<SDL_GamepadButton>(bind.key))) {
                        action.state.down = true;
                        break;
                    }
                }
            } else if (std::holds_alternative<Uint8>(bind.key)) {
                Uint8 button = std::get<Uint8>(bind.key);
                if (mouseButtons_[button]) {
                    action.state.down = true;
                }
            } else {
                auto ax = std::get<ButtonBinding::Axis>(bind.key);
                for (auto* gp : gamepads_) {
                    float val = SDL_GetGamepadAxis(gp, ax.axis) / 32767.0f;
                    if (val >= ax.threshold) {
                        action.state.down = true;
                        break;
                    }
                }
            }
        }

        auto& st = action.state;
        if (st.down)
            st.heldTime += dt;
        else
            st.heldTime = 0.0f;
    }
}

bool Input::isDown(const std::string& action) const {
    auto it = actions_.find(action);
    return it != actions_.end() && it->second.state.down;
}

bool Input::justPressed(const std::string& action) const {
    auto it = actions_.find(action);
    return it != actions_.end() && it->second.state.down && !it->second.state.prevDown;
}

bool Input::justReleased(const std::string& action) const {
    auto it = actions_.find(action);
    return it != actions_.end() && !it->second.state.down && it->second.state.prevDown;
}

bool Input::isKeyDown(SDL_Scancode scancode) const {
    const bool* keys = SDL_GetKeyboardState(nullptr);
    return keys[scancode];
}
