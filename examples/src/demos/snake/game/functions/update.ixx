module;

#include <cstdint>

export module snake.game.update;

import core.scheduler;
import core.time.FixedTimer;

import extensions.scheduler.accessors.events;
import extensions.scheduler.accessors.resources;
import extensions.scheduler.accessors.states;

import snake.game.adjust_snake_speed;
import snake.game.apple_spawn_rate;
import snake.game.AppleDigested;
import snake.game.color_cells;
import snake.game.eat_apple;
import snake.game.GameState;
import snake.game.move_snake;
import snake.game.spawn_apple;
import snake.game.trigger_apple_digested_event;

using namespace extensions::scheduler::accessors;

auto update_timers(
    resources::Resource<core::time::FixedTimer<game::apple_spawn_rate>> apple_spawn_timer,
    states::State<game::GameState>                                      game_state
) -> void;

[[nodiscard]]
auto number_of_snake_moves(states::State<game::GameState> game_state) -> uint32_t;

[[nodiscard]]
auto apple_was_digested(events::Reader<game::AppleDigested> event_reader) -> bool;

namespace game {

export inline constexpr auto update =
    core::scheduler::start_as(::update_timers)
        .then(
            core::scheduler::run_if(
                adjust_snake_speed,   //
                ::apple_was_digested
            )
        )
        .then(
            core::scheduler::repeat(
                core::scheduler::start_as(move_snake)   //
                    .then(eat_apple),
                ::number_of_snake_moves
            )
        )
        .then(
            core::scheduler::at_fixed_rate<apple_spawn_rate>(   //
                core::scheduler::group(
                    spawn_apple,                                //
                    trigger_apple_digested_event
                )
            )
        )
        .then(color_cells);

}   // namespace game

module :private;

auto update_timers(
    const resources::Resource<core::time::FixedTimer<game::apple_spawn_rate>>
                                         apple_spawn_timer,
    const states::State<game::GameState> game_state
) -> void
{
    apple_spawn_timer->update();
    game_state->snake_move_timer.update();
}

auto number_of_snake_moves(const states::State<game::GameState> game_state) -> uint32_t
{
    return game_state->snake_move_timer.delta_ticks();
}

auto apple_was_digested(events::Reader<game::AppleDigested> event_reader) -> bool
{
    return !event_reader.read().empty();
}
