import addons.ECS;

import core.app;
import core.measurement;
import core.scheduler;
import core.time;

import extensions.Addons;
import extensions.Assets;
import extensions.Events;
import extensions.Functional;
import extensions.Resources;
import extensions.scheduler;
import extensions.TaskRunner;

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
    core::scheduler::at_fixed_rate<window::display_rate>(   //
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

constexpr static auto shut_down = window::close_window;

auto main() -> int
{
    namespace argument_providers = extensions::scheduler::argument_providers;

    core::app::create()
        .extend_with(extensions::resources)
        .extend_with(extensions::assets)
        .extend_with(extensions::events)
        .extend_with(extensions::functional)
        .extend_with(extensions::addons)
        .use_addon(addons::ECS{})
        .extend_with(
            extensions::TaskRunner{
                argument_providers::resource_provider,
                argument_providers::event_provider,
                argument_providers::ecs,
            }
        )
        .transform(
            window::make_plugin(
                window::Settings{
                    .width  = 1'280,
                    .height = 720,
                    .title{ "Snake" },
                }
            )
        )
        .transform(
            game::make_plugin(
                game::Settings{
                    .cells_per_row    = 20,
                    .cells_per_column = 20,
                    .cell_width       = 32,
                }
            )
        )
        .run(
            core::scheduler::start_as(initialize)   //
                .then(run_game_loop)
                .then(shut_down)
        );
}
