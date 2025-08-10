module;

#include <utility>

export module ddge.modules.execution.primitives.at_fixed_rate;

import ddge.modules.time.FixedTimer;
import ddge.modules.time.VariableTimer;

import ddge.modules.execution.converts_to_task_builder_c;
import ddge.modules.execution.TaskBuilder;

import ddge.modules.execution.accessors.resources;
import ddge.modules.execution.primitives.repeat;

namespace ddge::exec {

/**
 * Requires resource of `Timer_T`
 **/
export template <typename Timer_T, converts_to_task_builder_c TaskBuilder_T>
    requires time::is_FixedTimer_c<Timer_T> || time::is_VariableTimer_c<Timer_T>
[[nodiscard]]
constexpr auto at_fixed_rate(TaskBuilder_T&& task_builder) -> TaskBuilder<void>;

}   // namespace ddge::exec

template <typename Timer_T, ddge::exec::converts_to_task_builder_c TaskBuilder_T>
    requires ddge::time::is_FixedTimer_c<Timer_T>
          || ddge::time::is_VariableTimer_c<Timer_T>
constexpr auto ddge::exec::at_fixed_rate(TaskBuilder_T&& task_builder)
    -> TaskBuilder<void>
{
    using FixedTimerResource = accessors::resources::Resource<Timer_T>;

    return repeat(
        std::forward<TaskBuilder_T>(task_builder),
        [](const FixedTimerResource timer) { return timer->delta_ticks(); }
    );
}
