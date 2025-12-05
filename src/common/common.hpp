// Taken from Rit

#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <SDL3/SDL.h>
#include <tuple>
#include <glad/glad.h>
#include <cctype>
#include <memory>
#include <vector>

#if defined(_WIN64)
    #define OS_WINDOWS
#elif defined(_WIN32)
    #define OS_WINDOWS
#elif defined(__APPLE__) || defined(__MACH__)
    #define OS_MACOS
#elif defined(__linux__)
    #define OS_LINUX
#endif

#define PI 3.14159265358979323846

namespace Common {

std::string toKeyName(SDL_Keycode keycode);

// String utilities
std::string uppercase(const std::string& str);
std::string titlecase(const std::string& str);
std::string lowercase(const std::string& str);
std::string trim(const std::string& str);
std::vector<std::string> split(const std::string& str, char delimiter);
std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
size_t hashString(const std::string& str);

// Functions for use with OpenGL and whatnot
// basically 640, 360 = 0.5, 0.5
// Mainly used for in-engine things, but y'know..
std::tuple<GLfloat, GLfloat> screenToGLCoords(int x, int y, int screenW, int screenH);
std::tuple<int, int> GLCoordsToScreen(float x, float y, int screenW, int screenH);

inline int width, height;

inline bool contextInitialized = false;

// Math
template<typename T>
constexpr T max(T a, T b) {
    return (a > b) ? a : b;
}

template<typename T>
constexpr T min(T a, T b) {
    return (a < b) ? a : b;
}

template<typename T>
constexpr T clamp(T value, T minVal, T maxVal) {
    return (value < minVal) ? minVal : (value > maxVal ? maxVal : value);
}

template<typename T>
constexpr T lerp(T a, T b, float t) {
    return a + (b - a) * t;
}

// Memory
size_t getMemoryUsage();
std::string formatBytes(size_t bytes);

// Build info
std::string getOSName();
std::string getBuildType();

} // namespace Common