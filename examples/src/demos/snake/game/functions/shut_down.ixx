export module snake.game.shut_down;

import extensions.scheduler.accessors.states.State;

import snake.game.GameState;

using namespace extensions::scheduler::accessors;

namespace game {

export inline constexpr auto shut_down = [](const states::State<GameState> game_state) {
    game_state.reset();
};

}   // namespace game
