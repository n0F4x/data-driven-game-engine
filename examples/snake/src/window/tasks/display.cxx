module;

#include <chrono>

module snake.window.tasks.display;

import ddge.modules.execution.accessors.resources;
import ddge.modules.execution.providers.ResourceProvider;
import ddge.modules.execution.v2.primitives.as_task;

import snake.window.Settings;
import snake.window.Window;

using namespace ddge::exec::accessors;

auto window::display()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>
{
    return ddge::exec::v2::as_task(
        [last_time = std::chrono::steady_clock::time_point{}](
            const resources::Resource<Settings> settings,
            const resources::Resource<Window>   window
        ) mutable -> void   //
        {
            window->display();

            using namespace std::chrono_literals;

            const auto now{ std::chrono::steady_clock::now() };
            const auto delta{ now - last_time };
            last_time = now;
            const auto fps{ 1s / delta };

            window->setTitle(std::format("{} - {:2d} FPS", settings->title, fps));
        }
    );
}
