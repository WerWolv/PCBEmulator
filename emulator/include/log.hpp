#pragma once

#include <fmt/core.h>
#include <fmt/color.h>

namespace vc::log {

    void debug(std::string_view fmt, auto ... args) {
#if defined(DEBUG)
        fmt::print(fg(fmt::color::green_yellow) | fmt::emphasis::bold, "[DEBUG] ");
        fmt::print(fmt, args...);
        fmt::print("\n");
        fflush(stdout);
#endif
    }

    void info(std::string_view fmt, auto ... args) {
        fmt::print(fg(fmt::color::cornflower_blue) | fmt::emphasis::bold, "[INFO]  ");
        fmt::print(fmt, args...);
        fmt::print("\n");
        fflush(stdout);
    }

    void warn(std::string_view fmt, auto ... args) {
        fmt::print(fg(fmt::color::light_golden_rod_yellow) | fmt::emphasis::bold, "[WARN]  ");
        fmt::print(fmt, args...);
        fmt::print("\n");
        fflush(stdout);
    }

    void error(std::string_view fmt, auto ... args) {
        fmt::print(fg(fmt::color::light_coral) | fmt::emphasis::bold, "[ERROR] ");
        fmt::print(fmt, args...);
        fmt::print("\n");
        fflush(stdout);
    }

    void fatal(std::string_view fmt, auto ... args) {
        fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "[FATAL] ");
        fmt::print(fmt, args...);
        fmt::print("\n");
        fflush(stdout);
    }

}