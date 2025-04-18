module;

#include <concepts>
#include <tuple>
#include <utility>

export module core.scheduler.task_wrappers.TaskGroup;

import utility.meta.algorithms.for_each;
import utility.meta.concepts.decays_to;
import utility.meta.type_traits.functional.arguments_of;

import core.scheduler.concepts.task_c;
import core.scheduler.task_wrappers.TaskWrapperBase;

namespace core::scheduler {

export template <task_c... Tasks_T>
class TaskGroup : public TaskWrapperBase {
public:
    template <typename... UTasks_T>
        requires(std::constructible_from<Tasks_T, UTasks_T> && ...)
    constexpr explicit TaskGroup(UTasks_T&&... tasks);

private:
    std::tuple<Tasks_T...> m_tasks;
};

}   // namespace core::scheduler

template <core::scheduler::task_c... Tasks_T>
template <typename... UTasks_T>
    requires(std::constructible_from<Tasks_T, UTasks_T> && ...)
constexpr core::scheduler::TaskGroup<Tasks_T...>::TaskGroup(UTasks_T&&... tasks)
    : m_tasks{ std::forward<UTasks_T>(tasks)... }
{}
