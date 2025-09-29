module;

#include <cstdint>

module snake.game.create_update_task_builder;

import ddge.modules.execution.TaskBuilder;
import ddge.modules.time.FixedTimer;

import ddge.modules.execution;

import snake.game.adjust_snake_speed;
import snake.game.AppleSpawnTimer;
import snake.game.AppleDigested;
import snake.game.color_cells;
import snake.game.create_eat_apple_task_builder;
import snake.game.GameState;
import snake.game.move_snake;
import snake.game.spawn_apple;
import snake.game.trigger_world_update_message;
import snake.game.WorldUpdate;

using namespace ddge::exec::accessors;

auto update_timers(
    const resources::Resource<game::AppleSpawnTimer> apple_spawn_timer,
    const states::State<game::GameState>             game_state
) -> void
{
    apple_spawn_timer->update();
    game_state->snake_move_timer.update();
}

auto number_of_snake_moves(const states::State<game::GameState> game_state) -> uint32_t
{
    return game_state->snake_move_timer.delta_ticks();
}

auto apple_was_digested(const events::Reader<game::AppleDigested> event_reader) -> bool
{
    return !event_reader.read().empty();
}

auto world_update_message_received(
    const messages::Receiver<game::WorldUpdate> message_receiver
) -> bool
{
    return !message_receiver.receive().empty();
}

auto game::update() -> ddge::exec::v2::TaskBuilder<void>
{
    return ddge::exec::v2::start_as(ddge::exec::v2::as_task(::update_timers))
        .then(
            ddge::exec::v2::run_if(
                ddge::exec::v2::as_task(adjust_snake_speed),   //
                ddge::exec::v2::as_task(::apple_was_digested)
            )
        )
        .then(
            ddge::exec::v2::repeat(
                ddge::exec::v2::group(
                    ddge::exec::v2::start_as(ddge::exec::v2::as_task(move_snake))   //
                        .then(eat_apple()),
                    ddge::exec::v2::as_task(trigger_world_update_message)
                ),
                ddge::exec::v2::as_task(::number_of_snake_moves)
            )
        )
        .then(
            ddge::exec::v2::at_fixed_rate<AppleSpawnTimer>(   //
                ddge::exec::v2::group(
                    ddge::exec::v2::as_task(spawn_apple),     //
                    ddge::exec::v2::as_task(trigger_world_update_message)
                )
            )
        )
        .then(
            ddge::exec::v2::run_if(
                ddge::exec::v2::as_task(color_cells),
                ddge::exec::v2::as_task(::world_update_message_received)
            )
        );
}
