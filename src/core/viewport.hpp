#pragma once
#include <SDL3/SDL.h>

namespace Core {
struct Viewport {
    int x, y;
    int w, h;
};

Viewport calculateViewport(int windowW, int windowH, int targetW, int targetH);

} // namespace Core