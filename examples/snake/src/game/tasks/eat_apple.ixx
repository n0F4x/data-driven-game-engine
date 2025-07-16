module;

#include <span>

export module snake.game.eat_apple;

import core.ecs;
import core.scheduler;

import extensions.scheduler.accessors.ecs;
import extensions.scheduler.accessors.events;
import extensions.scheduler.accessors.messages;

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
    Query<core::ecs::ID, With<game::SnakeHead>, With<game::Apple>> eaten_apples,
    Recorder<game::AppleDigested>                                  event_recorder,
    Sender<game::DigestedApple>                                    message_sender
) -> void;

auto despawn_digested_apple(
    Registry                      registry,
    Receiver<game::DigestedApple> message_receiver
) -> void;

auto grow_snake(
    Query<game::Snake>            snake_body,
    Receiver<game::DigestedApple> message_receiver
) -> void;

namespace game {

export inline constexpr auto eat_apple =   //
    core::scheduler::start_as(check_apple_digestion)
        .then(
            core::scheduler::group(
                despawn_digested_apple,   //
                grow_snake
            )
        );

}   // namespace game

module :private;

auto check_apple_digestion(
    const Query<core::ecs::ID, With<game::SnakeHead>, With<game::Apple>> eaten_apples,
    const Recorder<game::AppleDigested>                                  event_recorder,
    const Sender<game::DigestedApple>                                    message_sender
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
    const Query<game::Snake>            snake_body,
    const Receiver<game::DigestedApple> message_receiver
) -> void
{
    for (const auto _ : message_receiver.receive()) {
        snake_body.for_each([](game::Snake& snake) { snake.charge++; });
    }
}
