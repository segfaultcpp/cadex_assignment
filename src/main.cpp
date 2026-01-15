#include "eh.hpp"
#include "curve.hpp"
#include "parser.hpp"

#include <span>
#include <print>
#include <memory>
#include <ranges>
#include <vector>
#include <format>
#include <numbers>
#include <algorithm>
#include <filesystem>

namespace cadex {
    cadex::FloatType get_radius(std::unique_ptr<cadex::ACurve>& curve) {
        if (auto* ptr = dynamic_cast<cadex::Circle*>(curve.get())) {
            return ptr->Data().Radius;
        }
        if (auto* ptr = dynamic_cast<cadex::Ellipse*>(curve.get())) {
            cadex::EllipseData data = ptr->Data();
            return data.MajorRadius + data.MinorRadius;
        }
        if (auto* ptr = dynamic_cast<cadex::Helix*>(curve.get())) {
            return ptr->Data().Radius;
        }
        std::unreachable();
    }

    struct CommandLineArgs {
        std::filesystem::path input_file;
        ErrorPolicy error_policy = cadex::ErrorPolicy::SKIP;

        explicit CommandLineArgs(std::span<const char*> args) {
            cadex::ensure(args.size() > 1, "No input file");
            cadex::ensure(args.size() <= 3, "Too many arguments");

            if (args.size() == 2 && std::string_view(args[1]) == "help") {
                print_help();
                std::exit(0);
            }

            if (args.size() > 2) {
                std::string_view policy = args[1];
                if (policy == "--error-policy=skip") {
                    error_policy = cadex::ErrorPolicy::SKIP;
                } else if (policy == "--error-policy=panic") {
                    error_policy = cadex::ErrorPolicy::PANIC;
                } else {
                    print_help();
                    panic("Invalid CLI argument {}", args[1]);
                }
            }

            input_file = args.back();
        }

        static void print_help() {
            std::println("Usage: cadex [--error-policy={{skip, panic}}] INPUT_FILE");
            std::println("--error-policy: Specifies what parser should do if invalid token is encountered.");
            std::println("\tskip: The parser will skip errorneous line and continue parse input file. The program will be terminated if no valid curves found.");
            std::println("\tpanic: The program will be terminated if error is found.");
        }
    };
}

int main(int argc, const char** argv) {
    cadex::CommandLineArgs cli(std::span(argv, argc));

    cadex::Parser parser(cli.error_policy);
    auto curves = parser.Parse(cli.input_file);
    cadex::ensure(!curves.empty(), "No valid curves found");
    
    constexpr auto t = std::numbers::pi_v<cadex::FloatType> / cadex::FloatType(4);
    for (const auto& curve : curves) {
        std::println("Curve {{ ID: {}, Name: {} }}", curve->Id(), curve->Name());
        std::println("Point(PI / 4) = {}, Der(PI / 4) = {}\n", curve->Point(t), curve->Der(t));
    }
    
    std::vector<cadex::Circle*> circles = curves 
        | std::views::transform([](auto& curve) {
                return dynamic_cast<cadex::Circle*>(curve.get());
            })
        | std::views::filter([](cadex::Circle* ptr) {
                return ptr != nullptr;
            })
        | std::ranges::to<std::vector>();
    
    if (!circles.empty()) {
        std::ranges::sort(circles, std::ranges::less{}, [](const cadex::Circle* circle) {
            return circle->Data().Radius;
        });
        std::println("Circles' radii");
        for (cadex::Circle* circle : circles) {
            std::println("Circle {{ ID: {}, Radius: {} }}", circle->Id(), circle->Data().Radius);
        }
        std::println("");
    } else {
        std::println("No circles found");
    }

    const auto radii_sum = std::ranges::fold_left(curves | std::views::transform(cadex::get_radius), 0.f, std::plus<cadex::FloatType>{});
    std::println("Total sum of radii of all curves = {}", radii_sum);
    
    return 0;
}
