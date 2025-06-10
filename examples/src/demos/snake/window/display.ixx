module;

#include <chrono>

export module snake.window.display;

import extensions.scheduler.accessors.resources;

import snake.window.Settings;
import snake.window.Window;

using namespace extensions::scheduler::accessors;

namespace window {

export inline constexpr auto display =   //
    [last_time = std::chrono::steady_clock::time_point{}](
        const resources::Ref<Settings> settings,
        const resources::Ref<Window>   window
    ) mutable   //
{
    window->display();

    using namespace std::chrono_literals;

    const auto now{ std::chrono::steady_clock::now() };
    const auto delta{ now - last_time };
    last_time = now;
    const auto fps{ 1s / delta };

    window->set_title(std::format("{} - {:2d} FPS", settings->title, fps));
};

}   // namespace window
