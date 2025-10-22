// TODO: remove once Clang can mangle
#include <function2/function2.hpp>

import ddge.prelude;
import ddge.modules.time;
import ddge.modules.execution;
import ddge.utility.not_fn;

// TODO: import snake only once
import snake.assets;
import snake.game;
import snake.window;

using namespace ddge::exec::accessors;

[[nodiscard]]
auto initialize()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eMulti>
{
    return ddge::exec::v2::group(
        window::initialize(),   //
        game::initialize()
    );
}

auto process_events(const Processor& event_processor) -> void
{
    event_processor.process_events();
}

auto clear_messages(const Mailbox& mailbox) -> void
{
    mailbox.clear_messages();
}

[[nodiscard]]
auto update() -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eMulti>
{
    return ddge::exec::v2::group(
        window::update(),   //
        game::update()
    );
}

[[nodiscard]]
auto render() -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>
{
    return ddge::exec::v2::at_fixed_rate<window::DisplayTimer>(   //
        ddge::exec::v2::start_as(window::clear_window())          //
            .then(game::draw())
            .then(window::display())
    );
}

[[nodiscard]]
auto run_game_loop()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>
{
    return ddge::exec::v2::loop_until(
        ddge::exec::v2::start_as(
            ddge::exec::v2::group(
                ddge::exec::v2::as_task(process_events),   //
                ddge::exec::v2::as_task(clear_messages)
            )
        )
            .then(update())
            .then(render()),
        ddge::exec::v2::all_of(
            ddge::exec::v2::not_fn(window::window_should_close()),   //
            game::game_is_running()
        )
    );
}

[[nodiscard]]
auto shut_down()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>
{
    return ddge::exec::v2::start_as(game::shut_down())   //
        .then(window::close_window());
}

auto main() -> int
{
    ddge::app::create()
        .plug_in(ddge::plugins::Scheduler{})
        .plug_in(ddge::plugins::Resources{})
        .plug_in(ddge::plugins::States{})
        .plug_in(ddge::plugins::Events{})
        .plug_in(ddge::plugins::Messages{})
        .plug_in(ddge::plugins::ECS{})
        .plug_in(ddge::plugins::Assets{})
        .plug_in(ddge::plugins::Functional{})
        .transform(window::setup)
        .transform(game::setup)
        .run(
            ddge::exec::v2::start_as(initialize())   //
                .then(run_game_loop())
                .then(shut_down())
        );
}
