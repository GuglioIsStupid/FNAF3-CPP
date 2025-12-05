// Taken from Rit

#include "common.hpp"
#include <SDL3/SDL.h>
#include <string>
#include <tuple>
#include <glad/glad.h>
#include <cctype>
#include <memory>

#if defined(OS_WINDOWS)
    #include <windows.h>
    #include <psapi.h>
#elif defined(OS_LINUX)
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/sysinfo.h>
    #include <fstream>
    #include <sstream>
#elif defined(OS_MACOS)
    #include <mach/mach.h>
    #include <sys/types.h>
    #include <sys/sysctl.h>
    #include <unistd.h>
#endif

namespace Common {

std::string toKeyName(SDL_Keycode keycode) {
    switch (keycode) {
        case SDLK_0: return "0";
        case SDLK_1: return "1";
        case SDLK_2: return "2";
        case SDLK_3: return "3";
        case SDLK_4: return "4";
        case SDLK_5: return "5";
        case SDLK_6: return "6";
        case SDLK_7: return "7";
        case SDLK_8: return "8";
        case SDLK_9: return "9";
        
        case SDLK_A: return "a";
        case SDLK_B: return "b";
        case SDLK_C: return "c";
        case SDLK_D: return "d";
        case SDLK_E: return "e";
        case SDLK_F: return "f";
        case SDLK_G: return "g";
        case SDLK_H: return "h";
        case SDLK_I: return "i";
        case SDLK_J: return "j";
        case SDLK_K: return "k";
        case SDLK_L: return "l";
        case SDLK_M: return "m";
        case SDLK_N: return "n";
        case SDLK_O: return "o";
        case SDLK_P: return "p";
        case SDLK_Q: return "q";
        case SDLK_R: return "r";
        case SDLK_S: return "s";
        case SDLK_T: return "t";
        case SDLK_U: return "u";
        case SDLK_V: return "v";
        case SDLK_W: return "w";
        case SDLK_X: return "x";
        case SDLK_Y: return "y";
        case SDLK_Z: return "z";

        case SDLK_MINUS: return "-";
        case SDLK_EQUALS: return "=";
        case SDLK_GRAVE: return "`";
        case SDLK_LEFTBRACKET: return "[";
        case SDLK_RIGHTBRACKET: return "]";
        case SDLK_BACKSLASH: return "\\";
        case SDLK_SEMICOLON: return ";";
        case SDLK_APOSTROPHE: return "'";
        case SDLK_COMMA: return ",";
        case SDLK_PERIOD: return ".";
        case SDLK_SLASH: return "/";
        case SDLK_CAPSLOCK: return "capslock";

        case SDLK_ESCAPE: return "escape";
        case SDLK_SPACE: return "space";
        case SDLK_RETURN: return "enter";
        case SDLK_BACKSPACE: return "backspace";
        case SDLK_TAB: return "tab";
        case SDLK_LSHIFT: return "lshift";
        case SDLK_RSHIFT: return "rshift";
        case SDLK_LCTRL: return "lctrl";
        case SDLK_RCTRL: return "rctrl";
        case SDLK_LALT: return "lalt";
        case SDLK_RALT: return "ralt";
        case SDLK_UP: return "up";
        case SDLK_DOWN: return "down";
        case SDLK_LEFT: return "left";
        case SDLK_RIGHT: return "right";
        case SDLK_F1: return "f1";
        case SDLK_F2: return "f2";
        case SDLK_F3: return "f3";
        case SDLK_F4: return "f4";
        case SDLK_F5: return "f5";
        case SDLK_F6: return "f6";
        case SDLK_F7: return "f7";
        case SDLK_F8: return "f8";
        case SDLK_F9: return "f9";
        case SDLK_F10: return "f10";
        case SDLK_F11: return "f11";
        case SDLK_F12: return "f12";

        default: return "Unknown";
    }
}

std::string uppercase(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
    }
    return result;
}

std::string titlecase(const std::string& str) {
    if (str.empty()) return str;
    std::string result = str;
    result[0] = static_cast<char>(toupper(static_cast<unsigned char>(result[0])));
    for (size_t i = 1; i < result.size(); ++i) {
        result[i] = static_cast<char>(tolower(static_cast<unsigned char>(result[i])));
    }
    return result;
}

std::string lowercase(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    }
    return result;
}

std::string trim(const std::string& str) {
    const char* whitespace = " \t\n\r\f\v";
    size_t start = str.find_first_not_of(whitespace);
    if (start == std::string::npos) return ""; // All whitespace
    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);
    while (end != std::string::npos) {
        tokens.push_back(trim(str.substr(start, end - start)));
        start = end + 1;
        end = str.find(delimiter, start);
    }
    tokens.push_back(trim(str.substr(start)));
    return tokens;
}

std::string replaceAll(const std::string& str, const std::string& from, const std::string& to) {
    if (from.empty()) return str;
    std::string result = str;
    size_t startPos = 0;
    while ((startPos = result.find(from, startPos)) != std::string::npos) {
        result.replace(startPos, from.length(), to);
        startPos += to.length();
    }
    return result;
}

size_t hashString(const std::string& str) {
    size_t hash = 0xcbf29ce484222325;
    for (char c : str) {
        hash ^= static_cast<size_t>(c);
        hash *= 0x100000001b3;
    }
    return hash;
}

// GL
std::tuple<GLfloat, GLfloat> screenToGLCoords(int x, int y, int screenW, int screenH) {
    float glX = (2.0f * x) / screenW - 1.0f;
    float glY = 1.0f - (2.0f * y) / screenH;
    return {glX, glY};
}

std::tuple<int, int> GLCoordsToScreen(GLfloat x, GLfloat y, int screenW, int screenH) {
    int screenX = static_cast<int>(((x + 1.0f) / 2.0f) * screenW);
    int screenY = static_cast<int>(((1.0f - y) / 2.0f) * screenH);
    return {screenX, screenY};
}

// Math

// Memory
size_t getMemoryUsage() {
    #ifdef OS_WINDOWS
        // Windows-specific memory usage retrieval
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
            return static_cast<size_t>(pmc.WorkingSetSize);
        }
        return 0;
    #elif defined(OS_LINUX)
        // Linux-specific memory usage retrieval
        FILE* file = fopen("/proc/self/statm", "r");
        if (!file) return 0;

        long residentPages = 0;
        if (fscanf(file, "%*s %ld", &residentPages) != 1) {
            fclose(file);
            return 0;
        }
        fclose(file);

        long pageSize = sysconf(_SC_PAGESIZE);
        return static_cast<size_t>(residentPages * pageSize);
    #elif defined(OS_MACOS)
        // macOS-specific memory usage retrieval
        struct mach_task_basic_info info;
        mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
        if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &infoCount) == KERN_SUCCESS) {
            return static_cast<size_t>(info.resident_size);
        }
        return 0;
    #else
        return 0; // Unsupported platform
    #endif
}

std::string formatBytes(size_t bytes) {
    const char* sizes[] = { "B", "KB", "MB", "GB", "TB" };
    int order = 0;
    double len = static_cast<double>(bytes);
    while (len >= 1024.0 && order < 4) {
        order++;
        len /= 1024.0;
    }
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "%.2f %s", len, sizes[order]);
    return std::string(buffer);
}

// Build info
std::string getOSName() {
    #ifdef OS_WINDOWS
        return "Windows";
    #elif defined(OS_LINUX)
        return "Linux";
    #elif defined(OS_MACOS)
        return "macOS";
    #else
        return "Unknown";
    #endif
}

std::string getBuildType() {
    #ifdef DEBUG
        return "Debug";
    #else
        return "Release";
    #endif
}

} // namespace Common