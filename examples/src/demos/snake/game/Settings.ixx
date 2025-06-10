module;

#include <cstdint>

export module game.Settings;

namespace game {

export struct Settings {
    uint8_t  cells_per_row;
    uint8_t  cells_per_column;
    uint8_t  cell_width;
};

}   // namespace game
