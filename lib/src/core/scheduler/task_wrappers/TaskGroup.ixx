module;

#include <concepts>
#include <tuple>
#include <utility>

export module core.scheduler.task_wrappers.TaskGroup;

import utility.meta.algorithms.for_each;
import utility.meta.type_traits.type_list.type_list_concat;
import utility.meta.type_traits.type_list.type_list_unique;

import core.scheduler.concepts.task_c;
import core.scheduler.invoke_with_right_dependency_providers;
import core.scheduler.task_wrappers.TaskWrapperInterface;

namespace core::scheduler {

export template <task_c... Tasks_T>
class TaskGroup
    : public TaskWrapperInterface<util::meta::type_list_unique_t<
          util::meta::type_list_concat_t<typename Tasks_T::Dependencies...>>> {
public:
    template <typename... UTasks_T>
        requires(std::constructible_from<Tasks_T, UTasks_T &&> && ...)
    constexpr explicit TaskGroup(UTasks_T&&... tasks);

    template <typename Self_T, typename... DependencyProviders_T>
    constexpr auto operator()(this Self_T, DependencyProviders_T... dependency_providers)
        -> void;

private:
    std::tuple<Tasks_T...> m_tasks;
};

}   // namespace core::scheduler

template <core::scheduler::task_c... Tasks_T>
template <typename... UTasks_T>
    requires(std::constructible_from<Tasks_T, UTasks_T &&> && ...)
constexpr core::scheduler::TaskGroup<Tasks_T...>::TaskGroup(UTasks_T&&... tasks)
    : m_tasks{ std::forward<UTasks_T>(tasks)... }
{}

template <core::scheduler::task_c... Tasks_T>
template <typename Self_T, typename... DependencyProviders_T>
constexpr auto core::scheduler::TaskGroup<Tasks_T...>::operator()(
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
