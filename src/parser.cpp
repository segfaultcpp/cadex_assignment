#include "parser.hpp"

#include "curve.hpp"

#include <cctype>
#include <ranges>
#include <fstream>
#include <cstring>
#include <algorithm>

namespace cadex {
    std::vector<std::unique_ptr<ACurve>> Parser::Parse(const std::filesystem::path& file_path) const {
        std::ifstream fin(file_path);
        ensure(fin.is_open(), "Failed to open file with errno = {}", std::strerror(errno));

        size_t curves_num = 0;
        fin >> curves_num;

        std::vector<std::unique_ptr<cadex::ACurve>> curves;
        curves.reserve(curves_num);
        
        for (size_t line : std::views::iota(2u, curves_num + 2)) {
            char descriptor = 0;
            fin >> descriptor;
            if (!check_descriptor(descriptor)) {
                print_error(line, "Found invalid descriptor {}", descriptor);
                continue;
            }
            
            uint64_t id = 0;
            fin >> id;

            std::string name;
            fin >> name;
            if (!check_is_name_quoted(name)) {
                print_error(line, "Name must be quoted", descriptor);
            }
            name = name.substr(1);
            name.pop_back();

            if (!check_name(name)) {
                print_error(line, "Invalid name", descriptor);
                continue;
            }

            cadex::Vec3 origin;
            fin >> origin.X >> origin.Y >> origin.Z;

            if (descriptor == 'C') {
                cadex::CircleData data;
                fin >> data.Radius;
                if (data.Radius < 0.f) {
                    print_error(line, "Radius must be a positive number");
                    continue;
                }
                curves.push_back(std::make_unique<cadex::Circle>(std::move(name), id, origin, data));
            } 
            else if (descriptor == 'E') {
                cadex::EllipseData data;
                fin >> data.MinorRadius >> data.MajorRadius;
                if (data.MinorRadius < 0.f || data.MajorRadius < 0.f) {
                    print_error(line, "Radius must be a positive number");
                    continue;
                }
                curves.push_back(std::make_unique<cadex::Ellipse>(std::move(name), id, origin, data));
            } 
            else if (descriptor == 'H') {
                cadex::HelixData data;
                fin >> data.Radius >> data.Step;
                if (data.Radius < 0.f) {
                    print_error(line, "Radius must be a positive number");
                    continue;
                }
                curves.push_back(std::make_unique<cadex::Helix>(std::move(name), id, origin, data));
            } 
        }

        return curves;
    }

    bool Parser::check_descriptor(char descriptor) const {
        return descriptor == 'C' || descriptor == 'H' || descriptor == 'E';
    }

    bool Parser::check_is_name_quoted(std::string_view name) const {
        return name.starts_with("\"") && name.ends_with("\"");
    }
    
    bool Parser::check_name(std::string_view name) const {
        return std::ranges::any_of(name, [](char c) {
            return c == '_' || std::isalnum(c);
        });
    }
}
