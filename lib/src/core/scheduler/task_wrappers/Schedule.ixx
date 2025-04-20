module;

#include <concepts>
#include <tuple>
#include <type_traits>
#include <utility>

export module core.scheduler.task_wrappers.Schedule;

import utility.meta.algorithms.for_each;
import utility.meta.type_traits.forward_like;
import utility.meta.type_traits.type_list.type_list_concat;
import utility.meta.type_traits.type_list.type_list_unique;

import core.scheduler.as_task;
import core.scheduler.concepts.converts_to_task_c;
import core.scheduler.concepts.task_c;
import core.scheduler.invoke_with_right_dependency_providers;
import core.scheduler.task_wrappers.TaskWrapperInterface;

namespace core::scheduler {

export template <task_c... Tasks_T>
    requires(sizeof...(Tasks_T) != 0)
class Schedule : public TaskWrapperInterface<util::meta::type_list_unique_t<
                     util::meta::type_list_concat_t<typename Tasks_T::Dependencies...>>> {
public:
    template <typename... UTasks_T>
        requires(std::same_as<std::decay_t<UTasks_T>, Tasks_T> && ...)
    constexpr explicit Schedule(UTasks_T&&... tasks);

    template <converts_to_task_c Task_T, typename Self_T>
    constexpr auto then(this Self_T&&, Task_T&& task)
        -> Schedule<Tasks_T..., as_task_t<Task_T>>;

    template <typename Self_T, typename... DependencyProviders_T>
    constexpr auto operator()(this Self_T, DependencyProviders_T... dependency_providers)
        -> void;

private:
    std::tuple<Tasks_T...> m_tasks;
};

}   // namespace core::scheduler

template <core::scheduler::task_c... Tasks_T>
    requires(sizeof...(Tasks_T) != 0)
template <typename... UTasks_T>
    requires(std::same_as<std::decay_t<UTasks_T>, Tasks_T> && ...)
constexpr core::scheduler::Schedule<Tasks_T...>::Schedule(UTasks_T&&... tasks)
    : m_tasks{ std::forward<UTasks_T>(tasks)... }
{}

template <core::scheduler::task_c... Tasks_T>
    requires(sizeof...(Tasks_T) != 0)
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

template <core::scheduler::task_c... Tasks_T>
    requires(sizeof...(Tasks_T) != 0)
template <typename Self_T, typename... DependencyProviders_T>
constexpr auto core::scheduler::Schedule<Tasks_T...>::operator()(
    this Self_T self,
    DependencyProviders_T... dependency_providers
) -> void
{
    util::meta::for_each<std::make_index_sequence<sizeof...(Tasks_T)>>(
        [&self, &dependency_providers...]<size_t index_T> {
            invoke_with_right_dependency_providers(
                std::get<index_T>(self.m_tasks), dependency_providers...
            );
        }
    );
}
