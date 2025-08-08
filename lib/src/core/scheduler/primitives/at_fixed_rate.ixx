module;

#include <utility>

export module core.scheduler.primitives.at_fixed_rate;

import core.time.FixedTimer;
import core.time.VariableTimer;

import core.scheduler.converts_to_task_builder_c;
import core.scheduler.TaskBuilder;

import core.scheduler.accessors.resources;
import core.scheduler.primitives.repeat;

namespace core::scheduler {

/**
 * Requires resource of `Timer_T`
 **/
export template <typename Timer_T, core::scheduler::converts_to_task_builder_c TaskBuilder_T>
    requires core::time::is_FixedTimer_c<Timer_T>
          || core::time::is_VariableTimer_c<Timer_T>
[[nodiscard]]
constexpr auto at_fixed_rate(TaskBuilder_T&& task_builder)
    -> core::scheduler::TaskBuilder<void>;

}   // namespace core::scheduler

template <typename Timer_T, core::scheduler::converts_to_task_builder_c TaskBuilder_T>
    requires core::time::is_FixedTimer_c<Timer_T>
          || core::time::is_VariableTimer_c<Timer_T>
constexpr auto core::scheduler::at_fixed_rate(TaskBuilder_T&& task_builder)
    -> core::scheduler::TaskBuilder<void>
{
    using FixedTimerResource =
        core::scheduler::accessors::resources::Resource<Timer_T>;

    return core::scheduler::repeat(
        std::forward<TaskBuilder_T>(task_builder),
        [](const FixedTimerResource timer) { return timer->delta_ticks(); }
    );
}
