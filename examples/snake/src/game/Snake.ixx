module;

#include <cstdint>

export module snake.game.Snake;

import ddge.utility.Strong;

namespace game {

export struct Snake {
    uint8_t charge{};
};

}   // namespace game
