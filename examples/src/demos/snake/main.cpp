#include <chrono>
#include <format>
#include <string_view>

import addons.ecs;

import core.app;
import core.measurement;
import core.scheduler;
import core.time;

import demo.window;

import extensions.AddonManager;
import extensions.Functional;
import extensions.ResourceManager;
import extensions.scheduler;
import extensions.TaskRunner;

using namespace core::measurement::literals;
using namespace extensions::scheduler::accessors;

constexpr static std::string_view title = "Snake";

constexpr static auto initialize =                                   //
    [](const resources::Ref<window::Window>                 window,
       const resources::Ref<core::time::FixedTimer<60_ups>> timer)   //
{
    window->open();
    timer->reset();
};

constexpr static auto update_0 =   //
    [](const resources::Ref<window::Window>                 window,
       const window::EventRecorder&                         window_event_recorder,
       const events::Processor&                             event_processor,
       const resources::Ref<core::time::FixedTimer<60_ups>> timer)   //
{
    timer->update();
    window->record_events(window_event_recorder);
    event_processor.process_events();
};

constexpr static auto update_1 =                                        //
    [last_time = core::time::FixedTimer<60_ups>::Clock::time_point{}]   //
    (const events::Reader<window::events::CloseRequested>& close_requested_event_reader,
     const resources::Ref<window::Window>                  window,
     const resources::Ref<core::time::FixedTimer<60_ups>>  timer) mutable   //
{
    if (close_requested_event_reader.read().size() > 0) {
        window->close();
    }
    else if (timer->delta_ticks() >= 1) {
        using namespace std::chrono_literals;

        window->clear();
        window->display();

        const auto delta = timer->current() - last_time;
        last_time        = timer->current();
        const auto fps   = 1s / delta;

        window->set_title(std::format("{} - {:2d} FPS", title, fps));
    }
};

constexpr static auto game_is_running =                       //
    [](const resources::Ref<const window::Window> window) {   //
        return window->is_open();
    };

constexpr static auto run_game_loop = core::scheduler::loop_until(
    core::scheduler::in_parallel(update_0, update_1),
    game_is_running
);

auto main() -> int
{
    namespace argument_providers = extensions::scheduler::argument_providers;

    core::app::create()
        .extend_with(extensions::functional)
        .transform(
            window::make_plugin(
                window::Settings{ .width = 1'280, .height = 720, .title{ title } }
            )
        )
        .extend_with(extensions::addon_manager)
        .inject_addon(addons::make_ecs)
        .extend_with(
            extensions::TaskRunner{
                argument_providers::ResourceManager{},
                argument_providers::EventManager{},
                argument_providers::ECS{},
            }
        )
        .use_resource(core::time::FixedTimer<60_ups>{})
        .run(
            core::scheduler::start_as(initialize)   //
                .then(run_game_loop)
        );
}
