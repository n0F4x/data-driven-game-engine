import ddge.prelude;
import ddge.modules.time;
import ddge.modules.scheduler;
import ddge.utility.not_fn;

import snake;

using namespace ddge::scheduler::accessors;

static const ddge::scheduler::TaskBuilder<void> initialize =   //
    ddge::scheduler::group(
        window::initialize,                                    //
        game::create_initialize_task_builder()
    );

auto process_events(const events::Processor& event_processor) -> void
{
    event_processor.process_events();
}

auto clear_messages(const messages::Mailbox& mailbox) -> void
{
    mailbox.clear_messages();
}

static const ddge::scheduler::TaskBuilder<void> update =
    ddge::scheduler::group(window::update, game::create_update_task_builder());

static const ddge::scheduler::TaskBuilder<void> render =    //
    ddge::scheduler::at_fixed_rate<window::DisplayTimer>(   //
        ddge::scheduler::start_as(window::clear_window)     //
            .then(game::draw)
            .then(window::display)
    );

static const ddge::scheduler::TaskBuilder<void> run_game_loop =
    ddge::scheduler::loop_until(
        ddge::scheduler::start_as(
            ddge::scheduler::group(
                process_events,   //
                clear_messages
            )
        )
            .then(update)
            .then(render),
        ddge::scheduler::all_of(
            ddge::util::not_fn(window::window_should_close),   //
            game::game_is_running
        )
    );

static const ddge::scheduler::TaskBuilder<void> shut_down =
    ddge::scheduler::start_as(game::shut_down).then(window::close_window);

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
            ddge::scheduler::start_as(initialize)   //
                .then(run_game_loop)
                .then(shut_down)
        );
}
