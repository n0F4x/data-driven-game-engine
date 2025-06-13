module;

#include <concepts>
#include <tuple>
#include <type_traits>
#include <utility>

export module extensions.TaskRunner;

import utility.meta.concepts.specialization_of;

import core.app.builder_c;

import core.scheduler.build;
import core.scheduler.concepts.decays_to_task_builder_c;

namespace extensions {

export template <typename... ArgumentProviderBuilders_T>
class TaskRunner {
public:
    TaskRunner() = default;

    template <typename... UArgumentProviderBuilders_T>
        requires(
            std::constructible_from<ArgumentProviderBuilders_T, UArgumentProviderBuilders_T &&>
            && ...
        )
    constexpr explicit TaskRunner(
        UArgumentProviderBuilders_T&&... argument_provider_builders
    );

    template <
        core::app::builder_c                      Self_T,
        core::scheduler::decays_to_task_builder_c TaskBuilder_T>
    constexpr auto run(this Self_T&&, TaskBuilder_T&& task_builder) -> void;

private:
    std::tuple<ArgumentProviderBuilders_T...> m_argument_provider_builders;
};

template <typename... ArgumentProviderBuilders_T>
TaskRunner(ArgumentProviderBuilders_T&&...)
    -> TaskRunner<std::remove_cvref_t<ArgumentProviderBuilders_T>...>;

export inline constexpr TaskRunner<> task_runner;

}   // namespace extensions

template <typename... ArgumentProviderBuilders_T>
template <typename... UArgumentProviderBuilders_T>
    requires(
        std::constructible_from<ArgumentProviderBuilders_T, UArgumentProviderBuilders_T &&>
        && ...
    )
constexpr extensions::TaskRunner<ArgumentProviderBuilders_T...>::TaskRunner(
    UArgumentProviderBuilders_T&&... argument_provider_builders
)
    : m_argument_provider_builders{
          std::forward<UArgumentProviderBuilders_T>(argument_provider_builders)...
      }
{}

template <typename... ArgumentProviderBuilders_T>
template <core::app::builder_c Self_T, core::scheduler::decays_to_task_builder_c TaskBuilder_T>
constexpr auto extensions::TaskRunner<ArgumentProviderBuilders_T...>::run(
    this Self_T&&   self,
    TaskBuilder_T&& task_builder
) -> void
{
    auto app{ std::forward<Self_T>(self).build() };

    const auto build_task = [&self, &task_builder, &app] {
        return std::apply(
            [&task_builder, &app]<typename... XArgumentProviders_T>(
                XArgumentProviders_T&&... argument_provider_builders
            ) {
                return core::scheduler::build(
                    std::forward<TaskBuilder_T>(task_builder),
                    std::invoke(
                        std::forward<XArgumentProviders_T>(argument_provider_builders), app
                    )...
                );
            },
            std::forward_like<Self_T>(self.m_argument_provider_builders)
        );
    };

    std::invoke(build_task());
}
