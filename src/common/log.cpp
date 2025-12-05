// Taken from Rit

#include "log.hpp"
#include <iostream>
#include <string>
#include <common/common.hpp>

#define WARN_ANSI_COLOR "\033[1;33m"
#define ERROR_ANSI_COLOR "\033[1;31m"
#define INFO_ANSI_COLOR "\033[1;34m"
#define LOG_ANSI_COLOR "\033[1;32m"
#define RESET_ANSI_COLOR "\033[0m"

namespace Common {

void log(const std::string& message) {
    std::cout << LOG_ANSI_COLOR << "[LOG] " << RESET_ANSI_COLOR << message << std::endl;
}

void error(const std::string& message) {
    std::cerr << ERROR_ANSI_COLOR << "[ERROR] " << RESET_ANSI_COLOR << message << std::endl;
}

void warn(const std::string& message) {
    std::cout << WARN_ANSI_COLOR << "[WARN] " << RESET_ANSI_COLOR << message << std::endl;
}

void info(const std::string& message) {
    std::cout << INFO_ANSI_COLOR << "[INFO] " << RESET_ANSI_COLOR << message << std::endl;
}

void infoOS() {
    std::string os;
    #if defined(OS_WINDOWS)
        os = "Windows";
    #elif defined(OS_MACOS)
        os = "macOS";
    #elif defined(OS_LINUX)
        os = "Linux";
    #else
        os = "Unknown";
    #endif
    info("Running on " + os);
}

} // namespace Common