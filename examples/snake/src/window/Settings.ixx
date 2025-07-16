module;

#include <cstdint>
#include <string>

export module snake.window.Settings;

namespace window {

export struct Settings {
    uint16_t         width;
    uint16_t         height;
    std::string_view title;
};

}   // namespace window
