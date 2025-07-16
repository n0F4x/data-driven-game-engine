module;

#include <chrono>

export module snake.window.display_time;

using namespace std::chrono_literals;

namespace window {

export inline constexpr std::chrono::duration display_time{ 1s / 60.0 };

}   // namespace window
