module;

#include <concepts>
#include <utility>

export module core.scheduler.task_wrappers.LoopUntil;

import utility.meta.type_traits.type_list.type_list_concat;
import utility.meta.type_traits.type_list.type_list_unique;

import core.scheduler.concepts.task_c;
import core.scheduler.concepts.task_predicate_c;
import core.scheduler.invoke_with_right_dependency_providers;
import core.scheduler.task_wrappers.TaskWrapperInterface;

namespace core::scheduler {

export template <task_c Task_T, task_predicate_c Predicate_T>
class LoopUntil : public TaskWrapperInterface<
                      util::meta::type_list_unique_t<util::meta::type_list_concat_t<
                          typename Task_T::Dependencies,
                          typename Predicate_T::Dependencies>>> {
public:
    template <typename UTask_T, typename UPredicate_T>
        requires std::constructible_from<Task_T, UTask_T&&>
              && std::constructible_from<Predicate_T, UPredicate_T&&>
    constexpr explicit LoopUntil(UTask_T&& task, UPredicate_T&& predicate);

    template <typename Self_T, typename... DependencyProviders_T>
    constexpr auto operator()(this Self_T, DependencyProviders_T... dependency_providers)
        -> void;

private:
    Task_T      m_task;
    Predicate_T m_predicate;
};

}   // namespace core::scheduler

template <core::scheduler::task_c Task_T, core::scheduler::task_predicate_c Predicate_T>
template <typename UTask_T, typename UPredicate_T>
    requires std::constructible_from<Task_T, UTask_T&&>
              && std::constructible_from<Predicate_T, UPredicate_T&&>
constexpr core::scheduler::LoopUntil<Task_T, Predicate_T>::LoopUntil(
    UTask_T&&      task,
    UPredicate_T&& predicate
)
    : m_task{ std::forward<UTask_T>(task) },
      m_predicate{ std::forward<UPredicate_T>(predicate) }
{}

template <core::scheduler::task_c Task_T, core::scheduler::task_predicate_c Predicate_T>
template <typename Self_T, typename... DependencyProviders_T>
constexpr auto core::scheduler::LoopUntil<Task_T, Predicate_T>::operator()(
    this Self_T self,
    DependencyProviders_T... dependency_providers
) -> void
{
    const auto invoke_task = [&dependency_providers...](auto task) {
        invoke_with_right_dependency_providers(task, dependency_providers...);
    };
    const auto invoke_predicate = [&dependency_providers...](auto predicate) -> bool {
        return invoke_with_right_dependency_providers(predicate, dependency_providers...);
    };

    while (invoke_predicate(self.m_predicate)) {
        invoke_task(self.m_task);
    }
}
