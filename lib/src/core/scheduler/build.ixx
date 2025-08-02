module;

#include <functional>
#include <utility>

export module core.scheduler.build;

import core.scheduler.concepts.decays_to_task_builder_c;
import core.scheduler.provider_index_for_accessor;

import utility.meta.algorithms.apply;
import utility.meta.type_traits.integer_sequence.integer_sequence_unique;

namespace core::scheduler {

export template <decays_to_task_builder_c TaskBuilder_T, typename... Providers_T>
[[nodiscard]]
constexpr auto build(TaskBuilder_T&& task_builder, Providers_T&&... providers);

}   // namespace core::scheduler

template <typename TaskBuilder_T, typename... Providers_T>
using provider_indices_for_task_builder_t = util::meta::integer_sequence_unique_t<
    decltype(util::meta::apply<typename TaskBuilder_T::UniqueAccessors>(   //
        []<typename... XUniqueAccessors_T> {
            return std::index_sequence<
                core::scheduler::
                    provider_index_for_accessor<XUniqueAccessors_T, Providers_T...>...>{};
        }
    ))>;

template <core::scheduler::decays_to_task_builder_c TaskBuilder_T, typename... Providers_T>
constexpr auto
    core::scheduler::build(TaskBuilder_T&& task_builder, Providers_T&&... providers)
{
    return util::meta::apply<provider_indices_for_task_builder_t<
        std::remove_cvref_t<TaskBuilder_T>,
        Providers_T...>>([&task_builder,
                          &providers...]<std::size_t... provider_indices_T> {
        return std::invoke(
            std::forward<TaskBuilder_T>(task_builder),
            std::forward<Providers_T...[provider_indices_T]>(
                providers...[provider_indices_T]
            )...
        );
    });
}
