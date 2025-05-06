#include <chrono>
#include <format>
#include <string_view>

import addons.ecs;

import core.app;
import core.scheduler;
import core.time;

import demo.window;

import extensions.AddonManager;
import extensions.Functional;
import extensions.ResourceManager;
import extensions.scheduler;
import extensions.TaskRunner;

using namespace std::chrono_literals;

using namespace extensions::scheduler::accessors;

constexpr static std::string_view title = "Snake";

constexpr static auto initialize =                    //
    [](const resources::Ref<window::Window>   window,
       const resources::Ref<core::time::Time> time)   //
{
    window->open();
    time->reset();
};

constexpr static auto update_0 =   //
    [](const resources::Ref<window::Window>   window,
       const window::EventRecorder&           window_event_recorder,
       const events::Processor&               event_processor,
       const resources::Ref<core::time::Time> time)   //
{
    time->update(core::time::Time::Clock::now());
    window->record_events(window_event_recorder);
    event_processor.process_events();
};

constexpr static auto update_1 =   //
    [](const events::Reader<window::events::CloseRequested>& close_requested_event_reader,
       const resources::Ref<window::Window>                  window,
       const resources::Ref<core::time::Time>                time) mutable   //
{
    if (close_requested_event_reader.read().size() > 0) {
        window->close();
    }
    else {
        window->clear();
        window->display();
    }

    const auto fps = 1s / time->delta();

    window->set_title(std::format("{} - {:4d} FPS", title, fps));
};

constexpr static auto game_is_running =                       //
    [](const resources::Ref<const window::Window> window) {   //
        return window->is_open();
    };

constexpr static auto run_game_loop = core::scheduler::loop_until(
    core::scheduler::group(update_0, update_1),
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
        .use_resource(core::time::Time{})
        .run(
            core::scheduler::start_as(initialize)   //
                .then(run_game_loop)
        );
}
