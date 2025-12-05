#include "viewport.hpp"

namespace Core {
Viewport calculateViewport(int windowW, int windowH, int targetW, int targetH) {
    float targetAspect = (float)targetW / (float)targetH;
    float windowAspect = (float)windowW / (float)windowH;

    int vpW, vpH;
    if (windowAspect > targetAspect) {
        vpH = windowH;
        vpW = (int)(windowH * targetAspect);
    } else {
        vpW = windowW;
        vpH = (int)(windowW / targetAspect);
    }

    Viewport vp;
    vp.w = vpW;
    vp.h = vpH;
    vp.x = (windowW - vpW) / 2;
    vp.y = (windowH - vpH) / 2;
    return vp;
}
} // namespace Core