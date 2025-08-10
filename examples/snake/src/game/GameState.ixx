module;

#include <chrono>
#include <vector>

#include <SFML/Graphics.hpp>

export module snake.game.GameState;

import ddge.modules.assets.Handle;
import ddge.modules.states.state_c;
import ddge.modules.time.VariableTimer;

namespace game {

export struct GameState {
    constexpr static std::chrono::milliseconds default_snake_move_duration{ 500 };

    std::vector<ddge::assets::Handle<sf::Texture>>       textures;
    ddge::time::VariableTimer<std::chrono::milliseconds> snake_move_timer{
        default_snake_move_duration
    };
    std::chrono::milliseconds max_snake_move_duration{ 100 };
};

static_assert(ddge::states::state_c<GameState>);

}   // namespace game
