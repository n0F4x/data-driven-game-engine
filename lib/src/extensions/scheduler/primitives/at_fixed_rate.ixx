module;

#include <utility>

export module extensions.scheduler.primitives.at_fixed_rate;

import core.time.FixedTimer;
import core.time.VariableTimer;

import core.scheduler.converts_to_task_builder_c;
import core.scheduler.TaskBuilder;

import extensions.scheduler.accessors.resources;
import extensions.scheduler.primitives.repeat;

namespace extensions::scheduler {

/**
 * Requires resource of `Timer_T`
 **/
export template <typename Timer_T, core::scheduler::converts_to_task_builder_c TaskBuilder_T>
    requires core::time::is_FixedTimer_c<Timer_T>
          || core::time::is_VariableTimer_c<Timer_T>
[[nodiscard]]
constexpr auto at_fixed_rate(TaskBuilder_T&& task_builder)
    -> core::scheduler::TaskBuilder<void>;

}   // namespace extensions::scheduler

template <typename Timer_T, core::scheduler::converts_to_task_builder_c TaskBuilder_T>
    requires core::time::is_FixedTimer_c<Timer_T>
          || core::time::is_VariableTimer_c<Timer_T>
constexpr auto extensions::scheduler::at_fixed_rate(TaskBuilder_T&& task_builder)
    -> core::scheduler::TaskBuilder<void>
{
    using FixedTimerResource =
        extensions::scheduler::accessors::resources::Resource<Timer_T>;

    return extensions::scheduler::repeat(
        std::forward<TaskBuilder_T>(task_builder),
        [](const FixedTimerResource timer) { return timer->delta_ticks(); }
    );
}
