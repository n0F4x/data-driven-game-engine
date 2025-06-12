module;

#include <utility>

export module core.scheduler.at_fixed_rate;

import mp_units.core;

import core.measurement.tick_rate;

import core.time.FixedTimer;

import core.scheduler.repeat;
import core.scheduler.concepts.converts_to_task_builder_c;

import extensions.scheduler.accessors.resources;

namespace core::scheduler {

/**
 * Requires resource of `core::time::FixedTimer<tick_rate>`
 **/
export template <
    mp_units::QuantityOf<measurement::tick_rate> auto tick_rate,
    converts_to_task_builder_c                        TaskBuilder_T>
[[nodiscard]]
constexpr auto at_fixed_rate(TaskBuilder_T&& task_builder);

}   // namespace core::scheduler

template <
    mp_units::QuantityOf<core::measurement::tick_rate> auto tick_rate,
    core::scheduler::converts_to_task_builder_c             TaskBuilder_T>
constexpr auto core::scheduler::at_fixed_rate(TaskBuilder_T&& task_builder)
{
    using FixedTimerResource =
        extensions::scheduler::accessors::resources::Ref<core::time::FixedTimer<tick_rate>>;

    return core::scheduler::repeat(
        std::forward<TaskBuilder_T>(task_builder),
        [](const FixedTimerResource timer) { return timer->delta_ticks(); }
    );
}
