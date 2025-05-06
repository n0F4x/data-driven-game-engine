module;

#include <functional>
#include <utility>

export module core.scheduler.build;

import core.scheduler.concepts.decays_to_task_builder_c;
import core.scheduler.provider_index_for_argument;

import utility.meta.algorithms.apply;
import utility.meta.type_traits.integer_sequence.integer_sequence_unique;

namespace core::scheduler {

export template <decays_to_task_builder_c TaskBuilder_T, typename... ArgumentProviders_T>
[[nodiscard]]
constexpr auto
    build(TaskBuilder_T&& task_builder, ArgumentProviders_T&&... argument_providers);

}   // namespace core::scheduler

template <typename TaskBuilder_T, typename... ArgumentProviders_T>
using argument_provider_indices_for_task_builder_t =
    util::meta::integer_sequence_unique_t<decltype([] {
        return util::meta::apply<typename TaskBuilder_T::UniqueArguments>(
            []<typename... XUniqueArguments_T> {
                return std::index_sequence<core::scheduler::provider_index_for_argument<
                    XUniqueArguments_T,
                    ArgumentProviders_T...>...>{};
            }
        );
    }())>;

template <
    core::scheduler::decays_to_task_builder_c TaskBuilder_T,
    typename... ArgumentProviders_T>
constexpr auto core::scheduler::build(
    TaskBuilder_T&& task_builder,
    ArgumentProviders_T&&... argument_providers
)
{
    return util::meta::apply<argument_provider_indices_for_task_builder_t<
        std::remove_cvref_t<TaskBuilder_T>,
        ArgumentProviders_T...>>(
        [&task_builder, &argument_providers...]<size_t... argument_provider_indices_T> {
            return std::invoke(
                std::forward<TaskBuilder_T>(task_builder),
                std::forward<ArgumentProviders_T...[argument_provider_indices_T]>(
                    argument_providers...[argument_provider_indices_T]
                )...
            );
        }
    );
}
