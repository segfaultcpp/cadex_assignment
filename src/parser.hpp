#pragma once 

#include "eh.hpp"

#include <print>
#include <format>
#include <memory>
#include <vector>
#include <filesystem>

namespace cadex {
    class ACurve;
    
    enum class ErrorPolicy {
        SKIP,
        PANIC,
    };

    class Parser {
        ErrorPolicy mPolicy;

    public:
        explicit Parser(ErrorPolicy policy)
            : mPolicy(policy)
        {}
        
        std::vector<std::unique_ptr<ACurve>> Parse(const std::filesystem::path& file_path) const;

    private:
        template<typename... Args>
        void print_error(size_t line, std::format_string<Args...> msg, Args&&... args) const {
            auto formatted = std::format(msg, std::forward<Args>(args)...);
            if (mPolicy == ErrorPolicy::SKIP) {
                std::println("Error encountered while parsing input file on line {}: {}. Skipping this curve.", line, formatted);
            } else {
                panic("Error encountered while parsing input file on line {}: {}", line, formatted);
            }
        }
        
        bool check_descriptor(char descriptor) const;
        bool check_is_name_quoted(std::string_view name) const;
        bool check_name(std::string_view name) const;
    };
}
