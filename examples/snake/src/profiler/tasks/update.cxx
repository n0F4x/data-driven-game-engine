module;

#include <chrono>

module snake.profiler.tasks.update;

import ddge.modules.execution.accessors.resources;
import ddge.modules.execution.providers.ResourceProvider;
import ddge.modules.execution.v2.primitives.as_task;

import snake.profiler.SecondTimer;
import snake.window.DisplayInfo;

using namespace ddge::exec::accessors;

auto profiler::update()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>
{
    return ddge::exec::v2::as_task(
        [last_time = std::chrono::steady_clock::time_point{}, delta_update_count = 0u](
            const Resource<window::DisplayInfo> display_info,
            const Resource<SecondTimer>         second_timer
        ) mutable -> void {
            second_timer->update();
            ++delta_update_count;
            if (second_timer->delta_ticks() > 0) {
                display_info->average_ups = delta_update_count
                                          / second_timer->delta_ticks();
                delta_update_count = 0;
            }

            using namespace std::chrono_literals;

            const auto now{ std::chrono::steady_clock::now() };
            const auto delta{ now - last_time };
            last_time = now;
            const auto ups{ 1s / delta };

            display_info->ups = ups;
        }
    );
}
