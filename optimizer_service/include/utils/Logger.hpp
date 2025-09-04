#pragma once
#include <string>
#include <iostream>

class Logger {
private:
    static constexpr const char* RESET   = "\033[0m";
    static constexpr const char* RED     = "\033[31m";
    static constexpr const char* GREEN   = "\033[32m";
    static constexpr const char* YELLOW  = "\033[33m";
    static constexpr const char* BLUE    = "\033[34m";
    static constexpr const char* MAGENTA = "\033[35m";
    static constexpr const char* CYAN    = "\033[36m";
    static constexpr const char* WHITE   = "\033[37m";
public:
    static void debug(const std::string& msg) {
        std::cout << GREEN << "[DEBUG]" << RESET << " " << msg << std::endl;
    }

    static void info(const std::string& msg) {
        std::cout << YELLOW << "[INFO]" << RESET << " " << msg << std::endl;
    }

    static void error(const std::string& msg) {
        std::cerr << RED << "[ERROR]" << RESET << " " << msg << std::endl;
    }
};
