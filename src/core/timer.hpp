#pragma once
#include <SDL3/SDL.h>

namespace Core {
namespace Timer {

void init();
void tick();
float getDeltaTime();
float getFPS();

float getPreferredFrameTime();
void setPreferredFrameTime(float timeMs);

void limitFrameRate();

float getTime();

void step();

} // namespace Timer
} // namespace Core