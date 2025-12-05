#include "timer.hpp"
#include <SDL3/SDL.h>
#include <thread>
#include <chrono>

namespace Core {
namespace Timer {

static std::chrono::high_resolution_clock::time_point prevFrameTime;
static float deltaTime = 0.0f;
static float fps = 0.0f;
static float preferredFrameTime = 16.67f;
static bool initialized = false;

void init() {
    prevFrameTime = std::chrono::high_resolution_clock::now();
    deltaTime = 0.0f;
    fps = 0.0f;
    initialized = true;
}

void tick() {
    if (!initialized) init();

    auto current = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = current - prevFrameTime;
    deltaTime = elapsed.count();
    fps = (deltaTime > 0.0f) ? (1.0f / deltaTime) : 0.0f;

    prevFrameTime = current;
}

void limitFrameRate() {
    if (!initialized) return;

    auto current = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::milli> frameTime = current - prevFrameTime;
    float remaining = preferredFrameTime - frameTime.count();
    if (remaining > 0.0f) {
        std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(remaining));
    }
}

float getDeltaTime() { return deltaTime; }
float getFPS() { return fps; }
float getPreferredFrameTime() { return preferredFrameTime; }
void setPreferredFrameTime(float timeMs) { preferredFrameTime = timeMs; }

float getTime() {
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> timeSinceStart = now - prevFrameTime;
    return timeSinceStart.count();
}

void step() {
    // basically force reset deltatime (Useful for long load times)
    prevFrameTime = std::chrono::high_resolution_clock::now();
    deltaTime = 0.0f;
    fps = 0.0f;
}

} // namespace Timer
} // namespace Core
