module;

#include <cstdint>

module snake.game.tasks.update;

import ddge.modules.time.FixedTimer;

import ddge.modules.scheduler;

import snake.game.tasks.adjust_snake_speed;
import snake.game.AppleSpawnTimer;
import snake.game.AppleDigested;
import snake.game.tasks.color_cells;
import snake.game.tasks.eat_apple;
import snake.game.GameState;
import snake.game.tasks.move_snake;
import snake.game.tasks.spawn_apple;
import snake.game.tasks.trigger_world_update_message;
import snake.game.WorldUpdate;

using namespace ddge::scheduler::accessors;

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

auto game::tasks::update() -> ddge::scheduler::TaskBuilder<void>
{
    namespace sch = ddge::scheduler;

    return sch::start_as(sch::as_task(::update_timers))
        .then(
            sch::run_if(
                adjust_snake_speed(),   //
                sch::as_task(::apple_was_digested)
            )
        )
        .then(
            sch::repeat(
                sch::start_as(move_snake())   //
                    .then(eat_apple())
                    .then(trigger_world_update_message()),
                sch::as_task(::number_of_snake_moves)
            )
        )
        .then(
            sch::at_fixed_rate<AppleSpawnTimer>(   //
                sch::start_as(spawn_apple())       //
                    .then(trigger_world_update_message())
            )
        )
        .then(
            sch::run_if(
                color_cells(),   //
                sch::as_task(::world_update_message_received)
            )
        );
}
