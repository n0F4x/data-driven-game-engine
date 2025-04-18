module;

#include <utility>

export module core.scheduler.start_as;

import core.scheduler.as_task;
import core.scheduler.concepts.converts_to_task_c;
import core.scheduler.task_wrappers.Schedule;

namespace core::scheduler {

export template <converts_to_task_c Task_T>
[[nodiscard]]
constexpr auto start_as(Task_T&& task) -> Schedule<as_task_t<Task_T>>;

}   // namespace core::scheduler

template <core::scheduler::converts_to_task_c Task_T>
constexpr auto core::scheduler::start_as(Task_T&& task) -> Schedule<as_task_t<Task_T>>
{
    return Schedule<as_task_t<Task_T>>{ as_task_t<Task_T>{ std::forward<Task_T>(task) } };
}
