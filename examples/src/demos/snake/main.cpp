#include <chrono>
#include <format>
#include <string_view>

#include <SFML/Graphics/Color.hpp>

import addons.ecs;

import core.app;
import core.measurement;
import core.scheduler;
import core.time;

import demo.window;

import extensions.AddonManager;
import extensions.EventManager;
import extensions.Functional;
import extensions.ResourceManager;
import extensions.scheduler;
import extensions.TaskRunner;

import game;

using namespace core::measurement::literals;
using namespace extensions::scheduler::accessors;

template <typename T>
using Res = resources::Ref<T>;

constexpr static window::Settings window_settings{
    .width  = 1'280,
    .height = 720,
    .title{ "Snake" },
};

constexpr static auto initialize =                                                  //
    core::scheduler::start_as([](const Res<window::Window>                 window,
                                 const Res<core::time::FixedTimer<60_ups>> timer)   //
                              {
                                  window->open();
                                  timer->reset();
                              })
        .then(
            game::make_setup(
                game::Settings{
                    .window_width     = window_settings.width,
                    .window_height    = window_settings.height,
                    .cells_per_row    = 16,
                    .cells_per_column = 16,
                    .cell_width       = 36,
                }
            )
        );

constexpr static auto process_events =   //
    [](const Res<window::Window>                 window,
       const window::EventRecorder&              window_event_recorder,
       const events::Processor&                  event_processor,
       const Res<core::time::FixedTimer<60_ups>> timer)   //
{
    window->record_events(window_event_recorder);
    timer->update();
    event_processor.process_events();
};

constexpr static auto handle_window_events = core::scheduler::run_if(
    [](const Res<window::Window> window) { window->close(); },
    [](const events::Reader<window::events::CloseRequested>& closed_event_reader) {
        return closed_event_reader.read().size() > 0;
    }
);

constexpr static auto clear_window = [](const Res<window::Window> window) {
    window->clear(sf::Color{ 128, 0, 128 });
};

constexpr static auto display =   //
    [last_time = std::chrono::steady_clock::time_point{}](
        const Res<window::Window> window
    ) mutable   //
{
    window->display();

    using namespace std::chrono_literals;

    const auto now{ std::chrono::steady_clock::now() };
    const auto delta{ now - last_time };
    last_time = now;
    const auto fps{ 1s / delta };

    window->set_title(std::format("{} - {:2d} FPS", window_settings.title, fps));
};

constexpr static auto game_is_running =            //
    [](const Res<const window::Window> window) {   //
        return window->is_open();
    };

constexpr static auto run_game_loop = core::scheduler::loop_until(
    core::scheduler::start_as(process_events)
        .then(core::scheduler::group(handle_window_events, game::color_snake))
        .then(core::scheduler::at_fixed_rate<60_ups>(clear_window))
        .then(core::scheduler::at_fixed_rate<60_ups>(game::draw))
        .then(core::scheduler::at_fixed_rate<60_ups>(display)),
    game_is_running
);

auto main() -> int
{
    namespace argument_providers = extensions::scheduler::argument_providers;

    core::app::create()
        .extend_with(extensions::ResourceManager{})
        .extend_with(extensions::EventManager{})
        .extend_with(extensions::Functional{})
        .transform(window::make_plugin(window_settings))
        .use_resource(core::time::FixedTimer<60_ups>{})
        .extend_with(extensions::AddonManager{})
        .use_addon(addons::ECS{})
        .extend_with(
            extensions::TaskRunner{
                argument_providers::resource_provider,
                argument_providers::event_provider,
                argument_providers::ecs,
            }
        )
        .run(
            core::scheduler::start_as(initialize)   //
                .then(run_game_loop)
        );
}
