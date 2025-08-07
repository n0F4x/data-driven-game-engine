module;

#include <span>

module snake.game.create_eat_apple_task_builder;

import core.ecs;
import core.scheduler.TaskBuilder;

import extensions.scheduler;

import snake.game.Apple;
import snake.game.AppleDigested;
import snake.game.Cell;
import snake.game.DigestedApple;
import snake.game.Snake;
import snake.game.SnakeHead;

using namespace core::ecs::query_parameter_tags;
using namespace extensions::scheduler::accessors::ecs;
using namespace extensions::scheduler::accessors::events;
using namespace extensions::scheduler::accessors::messages;

auto check_apple_digestion(
    Query<core::ecs::ID, With<game::SnakeHead>, With<game::Apple>>& eaten_apples,
    const Recorder<game::AppleDigested>                             event_recorder,
    const Sender<game::DigestedApple>                               message_sender
) -> void
{
    eaten_apples.for_each([message_sender, event_recorder](const core::ecs::ID id) {
        event_recorder.record();
        message_sender.send(game::DigestedApple{ .id = id });
    });
}

auto despawn_digested_apple(
    const Registry                      registry,
    const Receiver<game::DigestedApple> message_receiver
) -> void
{
    for (const game::DigestedApple& digested_apple : message_receiver.receive()) {
        registry->remove<game::Apple>(digested_apple.id);
        registry->get_single<game::Cell>(digested_apple.id).shape.setTexture(nullptr);
    }
}

auto grow_snake(
    Query<game::Snake>&                 snake_body,
    const Receiver<game::DigestedApple> message_receiver
) -> void
{
    for (const auto _ : message_receiver.receive()) {
        snake_body.for_each([](game::Snake& snake) { snake.charge++; });
    }
}

auto game::create_eat_apple_task_builder() -> core::scheduler::TaskBuilder<void>
{
    return extensions::scheduler::start_as(check_apple_digestion)
        .then(
            extensions::scheduler::group(
                despawn_digested_apple,   //
                grow_snake
            )
        );
}
