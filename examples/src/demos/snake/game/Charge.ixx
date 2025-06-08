module;

#include <cstdint>

export module game.Charge;

import utility.Strong;

namespace game {

export struct Charge : util::Strong<unsigned, Charge> {
    using Strong::Strong;
};

}   // namespace game
