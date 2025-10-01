module;

#include <chrono>

export module snake.game.apple_spawn_duration;

using namespace std::chrono_literals;

namespace game {

export inline constexpr std::chrono::duration apple_spawn_duration{ 3s };

}   // namespace game
