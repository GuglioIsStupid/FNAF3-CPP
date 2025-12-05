#pragma once
#include <SDL3/SDL.h>
#include <unordered_map>
#include <vector>
#include <variant>
#include <string>
#include <algorithm>

namespace Core {
class Input {
public:
    enum class InputType {
        KEYBOARD,
        MOUSE
    };
    static Input& get();

    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    void processEvent(const SDL_Event& e);
    void update(float dt);

    bool isDown(const std::string& action) const;
    bool justPressed(const std::string& action) const;
    bool justReleased(const std::string& action) const;

    void addAction(const std::string& name);
    void bindKey(const std::string& action, SDL_Scancode scancode);
    void bindButton(const std::string& action, SDL_GamepadButton button);
    void bindMouseButton(const std::string& action, Uint8 button);
    void bindAxis(const std::string& action, SDL_GamepadAxis axis, float threshold);
    void setRepeat(const std::string& action, bool enabled, float delay = 0.5f, float interval = 0.05f);

    bool isKeyDown(SDL_Scancode scancode) const;

    int getMouseX() const { return mouseX_; }
    int getMouseY() const { return mouseY_; }

    int getMouseDeltaX() const { return mouseDeltaX_; }
    int getMouseDeltaY() const { return mouseDeltaY_; }
private:
    Input() = default;
    ~Input();

    struct ActionState {
        bool down = false;
        bool prevDown = false;
        float heldTime = 0.0f;
        bool repeatEnabled = false;
        float repeatDelay = 0.5f;
        float repeatInterval = 0.05f;
        float repeatTimer = 0.0f;
        bool repeatedThisFrame = false;
    };

    struct ButtonBinding {
        struct Axis { SDL_GamepadAxis axis; float threshold; };
        using BindVariant = std::variant<SDL_Scancode, SDL_GamepadButton, Axis, Uint8>;
        BindVariant key;
    };

    struct Action {
        std::vector<ButtonBinding> bindings;
        ActionState state;
    };

    std::unordered_map<std::string, Action> actions_;
    std::vector<SDL_Gamepad*> gamepads_;

    bool bindingMatches(const ButtonBinding& b) const;
    void openGamepad(int deviceIndex);
    void closeGamepad(SDL_JoystickID id);

    bool mouseButtons_[8] = {false};
    bool mouseButtonsPrev_[8] = {false};
    int mouseX_ = 0, mouseY_ = 0;
    int mouseDeltaX_ = 0, mouseDeltaY_ = 0;
};

extern Input& input;

} // namespace Core
