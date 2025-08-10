import prelude;

import modules.scheduler.TaskBuilder;
import modules.time;

import modules.scheduler;

import utility.not_fn;

import snake;

using namespace modules::scheduler::accessors;

static const modules::scheduler::TaskBuilder<void> initialize =   //
    modules::scheduler::group(
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

static const modules::scheduler::TaskBuilder<void> update =
    modules::scheduler::group(window::update, game::create_update_task_builder());

static const modules::scheduler::TaskBuilder<void> render =          //
    modules::scheduler::at_fixed_rate<window::DisplayTimer>(   //
        modules::scheduler::start_as(window::clear_window)     //
            .then(game::draw)
            .then(window::display)
    );

static const modules::scheduler::TaskBuilder<void> run_game_loop =
    modules::scheduler::loop_until(
        modules::scheduler::start_as(
            modules::scheduler::group(
                process_events,   //
                clear_messages
            )
        )
            .then(update)
            .then(render),
        modules::scheduler::all_of(
            util::not_fn(window::window_should_close),   //
            game::game_is_running
        )
    );

static const modules::scheduler::TaskBuilder<void> shut_down =
    modules::scheduler::start_as(game::shut_down).then(window::close_window);

auto main() -> int
{
    app::create()
        .plug_in(plugins::Scheduler{})
        .plug_in(plugins::Resources{})
        .plug_in(plugins::States{})
        .plug_in(plugins::Events{})
        .plug_in(plugins::Messages{})
        .plug_in(plugins::ECS{})
        .plug_in(plugins::Assets{})
        .plug_in(plugins::Functional{})
        .transform(window::setup)
        .transform(game::setup)
        .run(
            modules::scheduler::start_as(initialize)   //
                .then(run_game_loop)
                .then(shut_down)
        );
}
