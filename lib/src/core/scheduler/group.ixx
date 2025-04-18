module;

#include <tuple>
#include <type_traits>

export module core.scheduler.group;

import core.scheduler.as_task;
import core.scheduler.concepts.converts_to_task_c;
import core.scheduler.task_wrappers.TaskGroup;

namespace core::scheduler {

export template <converts_to_task_c... Tasks_T>
[[nodiscard]]
constexpr auto group(Tasks_T&&... tasks) -> TaskGroup<as_task_t<Tasks_T>...>;

}   // namespace core::scheduler

template <core::scheduler::converts_to_task_c... Tasks_T>
constexpr auto core::scheduler::group(Tasks_T&&... tasks)
    -> TaskGroup<as_task_t<Tasks_T>...>
{
    return TaskGroup<as_task_t<Tasks_T>...>{ std::forward<Tasks_T>(tasks)... };
}
