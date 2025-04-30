module;

#include <cstdint>
#include <string>

export module demo.window.Settings;

namespace window {

export struct Settings {
    uint16_t    width;
    uint16_t    height;
    std::string title;
};

}   // namespace window
