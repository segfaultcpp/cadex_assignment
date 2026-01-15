#pragma once

#include <print>
#include <format>
#include <cstdlib>

namespace cadex {
    template<typename... Args>
    [[noreturn]]
    void panic(std::format_string<Args...> msg, Args&&... args) {
        auto formatted = std::format(msg, std::forward<Args>(args)...);
        std::println("Application panicked with error message \"{}\"", formatted);
        std::abort();
    }

    template<typename... Args>
    void ensure(bool expr, std::format_string<Args...> msg, Args&&... args) {
        if (!expr) {
            panic(msg, std::forward<Args>(args)...);
        }
    }
}
