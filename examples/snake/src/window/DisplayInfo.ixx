module;

#include <cstdint>

export module snake.window.DisplayInfo;

namespace window {

export struct DisplayInfo {
    int64_t fps{};
    int64_t ups{};
    int64_t average_ups{};
    int64_t average_fps{};
};

}   // namespace window
