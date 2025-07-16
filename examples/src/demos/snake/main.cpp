import addons.ECS;

import app;

import core.measurement;
import core.scheduler;
import core.time;

import extensions.scheduler;

import plugins.assets;
import plugins.ecs;
import plugins.events;
import plugins.functional;
import plugins.resources;
import plugins.scheduler;
import plugins.states;

import utility.not_fn;

import snake;

using namespace extensions::scheduler::accessors;

constexpr static auto initialize =   //
    core::scheduler::group(
        window::initialize,          //
        game::initialize
    );

constexpr static auto process_events =               //
    [](const events::Processor& event_processor) {   //
        event_processor.process_events();
    };

constexpr static auto update = core::scheduler::group(window::update, game::update);

constexpr static auto render =                              //
    core::scheduler::at_fixed_rate<window::DisplayTimer>(   //
        core::scheduler::start_as(window::clear_window)     //
            .then(game::draw)
            .then(window::display)
    );

constexpr static auto run_game_loop = core::scheduler::loop_until(
    core::scheduler::start_as(process_events)   //
        .then(update)
        .then(render),
    core::scheduler::all_of(
        util::not_fn(window::window_should_close),   //
        game::game_is_running
    )
);

constexpr static auto shut_down =
    core::scheduler::start_as(game::shut_down).then(window::close_window);

auto main() -> int
{
    namespace argument_providers = extensions::scheduler::argument_providers;

    app::create()
        .plug_in(plugins::resources)
        .plug_in(plugins::assets)
        .plug_in(plugins::events)
        .plug_in(plugins::ecs)
        .plug_in(plugins::scheduler)
        .plug_in(plugins::states)
        .plug_in(plugins::functional)
        .transform(window::setup)
        .transform(game::setup)
        .run(
            core::scheduler::start_as(initialize)   //
                .then(run_game_loop)
                .then(shut_down)
        );
}
