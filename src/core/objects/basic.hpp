#pragma once

namespace Core {
namespace Objects {

class Basic {
public:
    Basic() = default;
    virtual ~Basic() = default;
    int zorder = 0;

    virtual void update([[maybe_unused]] float dt) {}
    virtual void render() {}
};

} // namespace Core
} // namespace Objects