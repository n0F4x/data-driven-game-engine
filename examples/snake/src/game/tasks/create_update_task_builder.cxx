module;

#include <cstdint>

module snake.game.create_update_task_builder;

import core.scheduler.TaskBuilder;
import core.time.FixedTimer;

import core.scheduler;

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

using namespace core::scheduler::accessors;

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

auto game::create_update_task_builder() -> core::scheduler::TaskBuilder<void>
{
    return core::scheduler::start_as(::update_timers)
        .then(
            core::scheduler::run_if(
                adjust_snake_speed,   //
                ::apple_was_digested
            )
        )
        .then(
            core::scheduler::repeat(
                core::scheduler::group(
                    core::scheduler::start_as(move_snake)   //
                        .then(create_eat_apple_task_builder()),
                    trigger_world_update_message
                ),
                ::number_of_snake_moves
            )
        )
        .then(
            core::scheduler::at_fixed_rate<AppleSpawnTimer>(   //
                core::scheduler::group(
                    spawn_apple,                                     //
                    trigger_world_update_message
                )
            )
        )
        .then(core::scheduler::run_if(color_cells, ::world_update_message_received));
}
