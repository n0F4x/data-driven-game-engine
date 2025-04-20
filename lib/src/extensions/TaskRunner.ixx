module;

#include <concepts>
#include <tuple>
#include <type_traits>
#include <utility>

export module extensions.TaskRunner;

import utility.meta.concepts.specialization_of;

import core.app.builder_c;

import core.scheduler.concepts.decays_to_task_c;
import core.scheduler.TaskRunner;

namespace extensions {

export template <typename... DependencyProviderBuilders_T>
class TaskRunner {
public:
    template <typename... UDependencyProviderBuilders_T>
        requires(
            std::constructible_from<
                DependencyProviderBuilders_T,
                UDependencyProviderBuilders_T &&>
            && ...
        )
    constexpr explicit TaskRunner(
        UDependencyProviderBuilders_T&&... dependency_provider_builders
    );

    template <core::app::builder_c Self_T, core::scheduler::decays_to_task_c Task_T>
    constexpr auto run(this Self_T&&, Task_T&& task) -> void;

private:
    std::tuple<DependencyProviderBuilders_T...> m_dependency_provider_builders;
};

template <typename... DependencyProviderBuilders_T>
TaskRunner(DependencyProviderBuilders_T&&...)
    -> TaskRunner<std::remove_cvref_t<DependencyProviderBuilders_T>...>;

}   // namespace extensions

template <typename... DependencyProviderBuilders_T>
template <typename... UDependencyProviderBuilders_T>
    requires(
        std::constructible_from<DependencyProviderBuilders_T, UDependencyProviderBuilders_T &&>
        && ...
    )
constexpr extensions::TaskRunner<DependencyProviderBuilders_T...>::TaskRunner(
    UDependencyProviderBuilders_T&&... dependency_provider_builders
)
    : m_dependency_provider_builders{
          std::forward<UDependencyProviderBuilders_T>(dependency_provider_builders)...
      }
{}

template <typename... DependencyProviderBuilders_T>
template <core::app::builder_c Self_T, core::scheduler::decays_to_task_c Task_T>
constexpr auto extensions::TaskRunner<DependencyProviderBuilders_T...>::run(
    this Self_T&& self,
    Task_T&&      task
) -> void
{
    auto app{ std::forward<Self_T>(self).build() };

    core::scheduler::TaskRunner task_runner{ std::apply(
        [&app](auto&&... dependency_provider_builders) {
            return core::scheduler::TaskRunner{
                std::invoke(dependency_provider_builders, app)...
            };
        },
        std::forward_like<Self_T>(self.m_dependency_provider_builders)
    ) };

    task_runner.run(std::forward<Task_T>(task));
}
