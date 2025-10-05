module;

#include <span>

module snake.game.tasks.eat_apple;

import ddge.modules.ecs;
import ddge.modules.execution.TaskBuilder;

import ddge.modules.execution;

import snake.game.Apple;
import snake.game.AppleDigested;
import snake.game.Cell;
import snake.game.DigestedApple;
import snake.game.Snake;
import snake.game.SnakeHead;

using namespace ddge::ecs::query_filter_tags;
using namespace ddge::exec::accessors::ecs;
using namespace ddge::exec::accessors::events;
using namespace ddge::exec::accessors::messages;

auto check_apple_digestion(
    Query<ddge::ecs::ID, With<game::SnakeHead>, With<game::Apple>>& eaten_apples,
    const Recorder<game::AppleDigested>                             event_recorder,
    const Sender<game::DigestedApple>                               message_sender
) -> void
{
    eaten_apples.for_each([message_sender, event_recorder](const ddge::ecs::ID id) -> void {
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

auto game::eat_apple() -> ddge::exec::v2::TaskBuilder<void>
{
    return ddge::exec::v2::start_as(ddge::exec::v2::as_task(check_apple_digestion))
        .then(
            ddge::exec::v2::group(
                ddge::exec::v2::as_task(despawn_digested_apple),   //
                ddge::exec::v2::as_task(grow_snake)
            )
        );
}
