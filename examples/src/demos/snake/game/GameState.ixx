module;

#include <vector>

#include <SFML/Graphics.hpp>

export module snake.game.GameState;

import core.assets.Handle;
import core.states.state_c;

namespace game {

export struct GameState {
    std::vector<core::assets::Handle<sf::Texture>> textures;
};

static_assert(core::states::state_c<GameState>);

}   // namespace game
