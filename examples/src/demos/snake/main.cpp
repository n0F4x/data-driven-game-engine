import core;
import addons;
import extensions;
import utility;

import demo.window;

using namespace extensions::scheduler::accessors;

constexpr static auto initialize =                      //
    [](const resources::Ref<window::Window> window) {   //
        window->open();
    };

constexpr static auto update_0 = [](const resources::Ref<window::Window> window,
                                    const window::EventRecorder window_event_recorder) {
    window->record_events(window_event_recorder);
};

constexpr static auto update_1 = [](const events::Processor event_processor) {
    event_processor.process_events();
};

constexpr static auto update_2 =                        //
    [](const events::Reader<window::events::CloseRequested> close_requested_event_reader,
       const resources::Ref<window::Window>                 window) {   //
        if (close_requested_event_reader.read().size() > 0) {
            window->close();
        }
    };

constexpr static auto game_is_running =                       //
    [](const resources::Ref<const window::Window> window) {   //
        return window->is_open();
    };

constexpr static auto run_game_loop = core::scheduler::loop_until(
    core::scheduler::group(update_0, update_1, update_2),
    game_is_running
);

auto main() -> int
{
    namespace dependency_providers = extensions::scheduler::dependency_providers;

    core::app::create()
        .extend_with(extensions::functional)
        .transform(
            window::make_plugin(
                window::Settings{ .width = 1'280, .height = 920, .title = "Snake" }
            )
        )
        .extend_with(extensions::addon_manager)
        .inject_addon(addons::make_ecs)
        .extend_with(
            extensions::TaskRunner{ dependency_providers::ResourceManager{},
                                    dependency_providers::EventManager{} }
        )
        .run(
            core::scheduler::start_as(initialize)   //
                .then(run_game_loop)
        );
}
