module;

#include <concepts>
#include <tuple>
#include <type_traits>
#include <utility>

export module plugins.scheduler;

import utility.meta.concepts.specialization_of;

import app.builder_c;

import core.scheduler;

namespace plugins {

export template <typename... ArgumentProviderBuilders_T>
class Scheduler {
public:
    Scheduler() = default;

    template <typename... UArgumentProviderBuilders_T>
        requires(
            std::constructible_from<ArgumentProviderBuilders_T, UArgumentProviderBuilders_T &&>
            && ...
        )
    constexpr explicit Scheduler(
        UArgumentProviderBuilders_T&&... argument_provider_builders
    );

    template <
        app::builder_c                      Self_T,
        core::scheduler::decays_to_task_builder_c TaskBuilder_T>
    constexpr auto run(this Self_T&&, TaskBuilder_T&& task_builder) -> void;

private:
    std::tuple<ArgumentProviderBuilders_T...> m_argument_provider_builders;
};

template <typename... ArgumentProviderBuilders_T>
Scheduler(ArgumentProviderBuilders_T&&...)
    -> Scheduler<std::remove_cvref_t<ArgumentProviderBuilders_T>...>;

namespace symbols {

export inline constexpr Scheduler<> scheduler;

}   // namespace symbols

}   // namespace plugins

template <typename... ArgumentProviderBuilders_T>
template <typename... UArgumentProviderBuilders_T>
    requires(
        std::constructible_from<ArgumentProviderBuilders_T, UArgumentProviderBuilders_T &&>
        && ...
    )
constexpr plugins::Scheduler<ArgumentProviderBuilders_T...>::Scheduler(
    UArgumentProviderBuilders_T&&... argument_provider_builders
)
    : m_argument_provider_builders{
          std::forward<UArgumentProviderBuilders_T>(argument_provider_builders)...
      }
{}

template <typename... ArgumentProviderBuilders_T>
template <app::builder_c Self_T, core::scheduler::decays_to_task_builder_c TaskBuilder_T>
constexpr auto plugins::Scheduler<ArgumentProviderBuilders_T...>::run(
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
