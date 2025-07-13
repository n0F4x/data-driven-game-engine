export module snake.game.shutdown;

import extensions.scheduler.accessors.states.State;

import snake.game.GameState;

using namespace extensions::scheduler::accessors;

namespace game {

export inline constexpr auto shutdown = [](states::State<GameState> game_state) {
    game_state.reset();
};

}   // namespace game
