import addons.ECS;

import app;

import core.scheduler.TaskBuilder;
import core.time;

import extensions.scheduler;

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

using namespace extensions::scheduler::accessors;

static const core::scheduler::TaskBuilder<void> initialize =   //
    extensions::scheduler::group(
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
    extensions::scheduler::group(window::update, game::create_update_task_builder());

static const core::scheduler::TaskBuilder<void> render =          //
    extensions::scheduler::at_fixed_rate<window::DisplayTimer>(   //
        extensions::scheduler::start_as(window::clear_window)     //
            .then(game::draw)
            .then(window::display)
    );

static const core::scheduler::TaskBuilder<void> run_game_loop =
    extensions::scheduler::loop_until(
        extensions::scheduler::start_as(
            extensions::scheduler::group(
                process_events,   //
                clear_messages
            )
        )
            .then(update)
            .then(render),
        extensions::scheduler::all_of(
            util::not_fn(window::window_should_close),   //
            game::game_is_running
        )
    );

static const core::scheduler::TaskBuilder<void> shut_down =
    extensions::scheduler::start_as(game::shut_down).then(window::close_window);

auto main() -> int
{
    app::create()
        .plug_in(plugins::scheduler)
        .plug_in(plugins::Resources{})
        .plug_in(plugins::states)
        .plug_in(plugins::events)
        .plug_in(plugins::messages)
        .plug_in(plugins::ecs)
        .plug_in(plugins::Assets{})
        .plug_in(plugins::functional)
        .transform(window::setup)
        .transform(game::setup)
        .run(
            extensions::scheduler::start_as(initialize)   //
                .then(run_game_loop)
                .then(shut_down)
        );
}
