module;

#include <utility>

export module ddge.modules.execution.v2.primitives.at_fixed_rate;

import ddge.modules.time.FixedTimer;
import ddge.modules.time.VariableTimer;

import ddge.modules.execution.accessors.resources;
import ddge.modules.execution.v2.primitives.as_task;
import ddge.modules.execution.v2.primitives.repeat;
import ddge.modules.execution.v2.TaskBuilder;

namespace ddge::exec::v2 {

/**
 * Requires resource of `Timer_T`
 **/
export template <typename Timer_T>
    requires time::is_FixedTimer_c<Timer_T> || time::is_VariableTimer_c<Timer_T>
[[nodiscard]]
constexpr auto at_fixed_rate(TaskBuilder<void>&& task_builder) -> TaskBuilder<void>
{
    using TimerResource = accessors::resources::Resource<Timer_T>;

    return repeat(
        std::move(task_builder),   //
        as_task([](const TimerResource timer) { return timer->delta_ticks(); })
    );
}

}   // namespace ddge::exec::v2
