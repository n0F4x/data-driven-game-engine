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

template <typename T>
using Res = resources::Ref<T>;

constexpr static std::string_view title = "Snake";

constexpr static auto initialize =                        //
    [](const Res<window::Window>                 window,
       const Res<core::time::FixedTimer<60_ups>> timer)   //
{
    window->open();
    timer->reset();
};

constexpr static auto process_events =   //
    [](const Res<window::Window>                 window,
       const window::EventRecorder&              window_event_recorder,
       const events::Processor&                  event_processor,
       const Res<core::time::FixedTimer<60_ups>> timer)   //
{
    timer->update();
    window->record_events(window_event_recorder);
    event_processor.process_events();
};

constexpr static auto update_window =
    [](const events::Reader<window::events::CloseRequested>& close_requested_event_reader,
       const Res<window::Window>                             window)   //
{
    if (close_requested_event_reader.read().size() > 0) {
        window->close();
    }
};

constexpr static auto draw =   //
    [last_time = std::chrono::steady_clock::time_point{}](
        const Res<window::Window> window
    ) mutable   //
{
    using namespace std::chrono_literals;

    window->clear();
    window->display();

    const auto now{ std::chrono::steady_clock::now() };
    const auto delta{ now - last_time };
    last_time = now;
    const auto fps{ 1s / delta };

    window->set_title(std::format("{} - {:2d} FPS", title, fps));
};

constexpr static auto fixed_draw =
    core::scheduler::repeat( //
        draw, //
        [](const Res<core::time::FixedTimer<60_ups>> timer) {
            return timer->delta_ticks();
        }
    );

constexpr static auto game_is_running =            //
    [](const Res<const window::Window> window) {   //
        return window->is_open();
    };

constexpr static auto run_game_loop = core::scheduler::loop_until(
    core::scheduler::in_parallel(process_events, update_window, fixed_draw),
    game_is_running
);

auto main() -> int
{
    namespace argument_providers = extensions::scheduler::argument_providers;

    core::app::create()
        .extend_with(extensions::Functional{})
        .transform(
            window::make_plugin(
                window::Settings{ .width = 1'280, .height = 720, .title{ title } }
            )
        )
        .extend_with(extensions::AddonManager{})
        .use_addon(addons::ECS{})
        .extend_with(
            extensions::TaskRunner{
                argument_providers::resource_provider,
                argument_providers::event_provider,
                argument_providers::ecs,
            }
        )
        .use_resource(core::time::FixedTimer<60_ups>{})
        .run(
            core::scheduler::start_as(initialize)   //
                .then(run_game_loop)
        );
}
