module;

#include <chrono>

export module snake.window.display_refresh_duration;

using namespace std::chrono_literals;

namespace window {

export inline constexpr std::chrono::duration display_refresh_duration{ 1s / 60.0 };

}   // namespace window
