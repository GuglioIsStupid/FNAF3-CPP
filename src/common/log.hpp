// Taken from Rit

#pragma once

#include <string>

namespace Common {

void log(const std::string& message);
void error(const std::string& message);
void warn(const std::string& message);
void info(const std::string& message);
void infoOS();

} // namespace Common