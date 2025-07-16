module;

#include <chrono>

export module snake.game.apple_spawn_time;

namespace game {

export inline constexpr std::chrono::milliseconds apple_spawn_time{ 3'000 };

}   // namespace game
