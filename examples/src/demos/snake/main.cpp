import core;
import addons;
import extensions;
import utility;

import demo.Window;

constexpr static auto initialize      = [] {};
constexpr static auto update_0        = [] {};
constexpr static auto update_1        = [] {};
constexpr static auto game_is_running = [] -> bool { return true; };

constexpr static auto run_game_loop = core::scheduler::loop_until(
    core::scheduler::group(update_0, update_1),
    game_is_running
);

auto main() -> int
{
    core::app::Builder{}   //
        .extend_with<extensions::ScheduleRunner>()
        .run(
            core::scheduler::start_as(initialize)   //
                .then(run_game_loop)
        );
}
