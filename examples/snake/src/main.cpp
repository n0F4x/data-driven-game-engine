import addons.ECS;

import app;

import core.scheduler.TaskBuilder;
import core.time;

import core.scheduler;

import plugins.assets;
import plugins.ecs;
import plugins.events;
import plugins.messages;
import plugins.functional;
import plugins.resources;
import plugins.scheduler;
import plugins.states;

import utility.not_fn;

import snake;

using namespace core::scheduler::accessors;

static const core::scheduler::TaskBuilder<void> initialize =   //
    core::scheduler::group(
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

static const core::scheduler::TaskBuilder<void> update =
    core::scheduler::group(window::update, game::create_update_task_builder());

static const core::scheduler::TaskBuilder<void> render =          //
    core::scheduler::at_fixed_rate<window::DisplayTimer>(   //
        core::scheduler::start_as(window::clear_window)     //
            .then(game::draw)
            .then(window::display)
    );

static const core::scheduler::TaskBuilder<void> run_game_loop =
    core::scheduler::loop_until(
        core::scheduler::start_as(
            core::scheduler::group(
                process_events,   //
                clear_messages
            )
        )
            .then(update)
            .then(render),
        core::scheduler::all_of(
            util::not_fn(window::window_should_close),   //
            game::game_is_running
        )
    );

static const core::scheduler::TaskBuilder<void> shut_down =
    core::scheduler::start_as(game::shut_down).then(window::close_window);

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
            core::scheduler::start_as(initialize)   //
                .then(run_game_loop)
                .then(shut_down)
        );
}
