module;

#include <chrono>
#include <vector>

#include <SFML/Graphics.hpp>

export module snake.game.GameState;

import core.assets.Handle;
import core.states.state_c;
import core.time.VariableTimer;

namespace game {

export struct GameState {
    constexpr static std::chrono::milliseconds default_snake_move_duration{ 500 };

    std::vector<core::assets::Handle<sf::Texture>>       textures;
    core::time::VariableTimer<std::chrono::milliseconds> snake_move_timer{
        default_snake_move_duration
    };
    std::chrono::milliseconds max_snake_move_duration{ 100 };
};

static_assert(core::states::state_c<GameState>);

}   // namespace game
