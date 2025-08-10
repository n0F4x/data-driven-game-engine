export module snake.game.shut_down;

import ddge.modules.execution.accessors.states.State;

import snake.game.GameState;

using namespace ddge::exec::accessors;

namespace game {

export auto shut_down(states::State<GameState> game_state) -> void;

}   // namespace game

auto game::shut_down(const states::State<GameState> game_state) -> void
{
    game_state.reset();
}
