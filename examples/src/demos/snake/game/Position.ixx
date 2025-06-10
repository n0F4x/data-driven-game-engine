module;

#include <compare>
#include <cstdint>

export module snake.game.Position;

namespace game {

export struct Position {
    uint8_t row{};
    uint8_t column{};

    auto operator<=>(const Position&) const = default;
};

}   // namespace game
