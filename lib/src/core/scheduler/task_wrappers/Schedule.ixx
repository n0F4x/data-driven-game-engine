module;

#include <concepts>
#include <tuple>
#include <type_traits>
#include <utility>

export module core.scheduler.task_wrappers.Schedule;

import utility.meta.type_traits.forward_like;

import core.scheduler.as_task;
import core.scheduler.concepts.converts_to_task_c;
import core.scheduler.concepts.task_c;
import core.scheduler.task_wrappers.TaskWrapperBase;

namespace core::scheduler {

export template <task_c... Tasks_T>
class Schedule : TaskWrapperBase {
public:
    template <typename... UTasks_T>
        requires(std::same_as<std::decay_t<UTasks_T>, Tasks_T> && ...)
    constexpr explicit Schedule(UTasks_T&&... tasks);

    template <converts_to_task_c Task_T, typename Self_T>
    constexpr auto then(this Self_T&&, Task_T&& task)
        -> Schedule<Tasks_T..., as_task_t<Task_T>>;

private:
    std::tuple<Tasks_T...> m_tasks;
};

}   // namespace core::scheduler

template <core::scheduler::task_c... Tasks_T>
template <typename... UTasks_T>
    requires(std::same_as<std::decay_t<UTasks_T>, Tasks_T> && ...)
constexpr core::scheduler::Schedule<Tasks_T...>::Schedule(UTasks_T&&... tasks)
    : m_tasks{ std::forward<UTasks_T>(tasks)... }
{}

template <core::scheduler::task_c... Tasks_T>
template <core::scheduler::converts_to_task_c Task_T, typename Self_T>
constexpr auto
    core::scheduler::Schedule<Tasks_T...>::then(this Self_T&& self, Task_T&& task)
        -> Schedule<Tasks_T..., as_task_t<Task_T>>
{
    return std::apply(
        [&task]<typename... XTasks_T>(XTasks_T&&... tasks) {
            return Schedule<Tasks_T..., as_task_t<Task_T>>{
                std::forward<XTasks_T>(tasks)...,
                as_task_t<Task_T>{ std::forward<Task_T>(task) }
            };
        },
        std::forward_like<Self_T>(self.m_tasks)
    );
}
