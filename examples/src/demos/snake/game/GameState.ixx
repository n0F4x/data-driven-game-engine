module;

#include <vector>

#include <SFML/Graphics.hpp>

export module snake.game.GameState;

import mp_units.core;

import core.assets.Handle;
import core.states.state_c;
import core.measurement.updates_per_second;
import core.time.VariableTimer;

using namespace core::measurement::symbols;

namespace game {

export struct GameState {
    constexpr static mp_units::quantity<ups> default_snake_speed{ 2 * ups };

    std::vector<core::assets::Handle<sf::Texture>> textures;
    core::time::VariableTimer<ups> snake_move_timer{ default_snake_speed };
    mp_units::quantity<ups>        max_snake_speed{ 10 * ups };
};

static_assert(core::states::state_c<GameState>);

}   // namespace game
