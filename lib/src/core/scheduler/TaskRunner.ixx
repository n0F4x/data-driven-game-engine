module;

#include <concepts>
#include <tuple>
#include <utility>

export module core.scheduler.TaskRunner;

import core.scheduler.concepts.decays_to_task_c;
import core.scheduler.invoke_with_right_dependency_providers;
import core.scheduler.task_wrappers.Schedule;

namespace core::scheduler {

export template <typename... DependencyProviders_T>
class TaskRunner {
public:
    template <typename... UDependencyProviders_T>
        requires(
            std::constructible_from<DependencyProviders_T, UDependencyProviders_T &&>
            && ...
        )
    constexpr explicit TaskRunner(UDependencyProviders_T&&... dependency_providers);

    template <typename Self_T, decays_to_task_c Task_T>
    constexpr auto run(this Self_T, Task_T&& task) -> void;

private:
    std::tuple<DependencyProviders_T...> m_dependency_providers;
};

template <typename... DependencyProviders_T>
TaskRunner(DependencyProviders_T&&...)
    -> TaskRunner<std::remove_cvref_t<DependencyProviders_T>...>;

}   // namespace core::scheduler

template <typename... DependencyProviders_T>
template <typename... UDependencyProviders_T>
    requires(
        std::constructible_from<DependencyProviders_T, UDependencyProviders_T &&> && ...
    )
constexpr core::scheduler::TaskRunner<DependencyProviders_T...>::TaskRunner(
    UDependencyProviders_T&&... dependency_providers
)
    : m_dependency_providers{ std::forward<UDependencyProviders_T>(dependency_providers
      )... }
{}

template <typename... DependencyProviders_T>
template <typename Self_T, core::scheduler::decays_to_task_c Task_T>
constexpr auto core::scheduler::TaskRunner<DependencyProviders_T...>::run(
    this Self_T self,
    Task_T&&    task
) -> void
{
    std::apply(
        [&task](auto... dependency_providers) {
            invoke_with_right_dependency_providers(
                std::forward<Task_T>(task), dependency_providers...
            );
        },
        self.m_dependency_providers
    );
}
