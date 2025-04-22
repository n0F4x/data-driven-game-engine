import core;
import addons;
import extensions;
import utility;

import demo.Window;

using namespace extensions::scheduler::accessors;

constexpr static auto initialize = [](const resources::Ref<Window> window) {
    window->open();
};

constexpr static auto update_0 = [] {};
constexpr static auto update_1 = [](const resources::Ref<Window> window) {
    window->close();
};

constexpr static auto game_is_running = [](const resources::Ref<const Window> window
                                        ) -> bool { return window->is_open(); };

constexpr static auto run_game_loop = core::scheduler::loop_until(
    core::scheduler::group(update_0, update_1),
    game_is_running
);

auto main() -> int
{
    namespace dependency_providers = extensions::scheduler::dependency_providers;

    core::app::create()
        .extend_with(extensions::ResourceManager{})
        .use_resource(Window{})
        .extend_with(extensions::TaskRunner{ dependency_providers::ResourceManager{} })
        .run(
            core::scheduler::start_as(initialize)   //
                .then(run_game_loop)
        );
}
