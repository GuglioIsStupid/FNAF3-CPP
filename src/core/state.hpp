#pragma once
#include <SDL3/SDL.h>
#include <core/objects/basic.hpp>
#include <vector>
#include <memory>
#include <algorithm>
#include <string>

namespace Core {

class Game;

class State {
public:
    bool allowScripting = false;
    std::string state_name = "unnamed";
    std::vector<std::shared_ptr<Objects::Basic>> members;
    bool autoSort = true;
    
    void add(std::shared_ptr<Objects::Basic> member, bool sort = false) {
        members.push_back(member);
        if (autoSort || sort) sortMembers();
    }

    void removeMember(std::shared_ptr<Objects::Basic> member) {
        members.erase(std::remove(members.begin(), members.end(), member), members.end());
    }

    void remove(std::shared_ptr<Objects::Basic> member) { // alias
        removeMember(member);
    }

    virtual ~State() = default;

    virtual void enter([[maybe_unused]] Game& game) {}

    virtual void handleEvents([[maybe_unused]] Game& game, [[maybe_unused]] SDL_Event& event) = 0;
    virtual void update([[maybe_unused]] Game& game, [[maybe_unused]] float dt) = 0;
    virtual void render([[maybe_unused]] Game& game) {
        for (auto& m : members) {
            m->render();
        }
    }

    virtual void onResize([[maybe_unused]] Game& game, [[maybe_unused]] int width, [[maybe_unused]] int height) {}
    virtual void leave([[maybe_unused]] Game& game) {}

private:
    void sortMembers() {
        std::sort(members.begin(), members.end(),
            [](const std::shared_ptr<Objects::Basic>& a, const std::shared_ptr<Objects::Basic>& b) {
                return a->zorder < b->zorder;
            }
        );
    }
};

} // namespace Core
