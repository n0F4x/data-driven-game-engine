#include <concepts>
#include <type_traits>

import core;
import addons;
import extensions;
import utility;

import demo.Window;

constexpr static auto initialize      = [] {};
constexpr static auto update_0        = [] {};
constexpr static auto update_1        = [] {};
constexpr static auto game_is_running = [](const int v) -> bool { return v != 42; };

constexpr static auto run_game_loop = core::scheduler::loop_until(
    core::scheduler::group(update_0, update_1),
    game_is_running
);

struct IntProvider {
    template <std::same_as<int>>
    [[nodiscard]]
    constexpr static auto provide() -> int
    {
        return 42;
    }
};

auto main() -> int
{
    core::app::create()
        .extend_with(extensions::TaskRunner{ [](auto) { return IntProvider{}; } })
        .run(
            core::scheduler::start_as(initialize)   //
                .then(run_game_loop)
        );
}
