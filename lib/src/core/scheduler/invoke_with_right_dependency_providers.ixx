module;

#include <utility>

export module core.scheduler.invoke_with_right_dependency_providers;

import utility.meta.algorithms.any_of;
import utility.meta.algorithms.apply;
import utility.meta.type_traits.integer_sequence.index_sequence_filter;
import utility.meta.type_traits.type_list.type_list_at;
import utility.TypeList;

import core.scheduler.concepts.decays_to_task_c;
import core.scheduler.concepts.provides_dependency_c;

namespace core::scheduler {

export template <decays_to_task_c Task_T, typename... DependencyProviders_T>
constexpr auto invoke_with_right_dependency_providers(
    Task_T&& task,
    DependencyProviders_T... dependency_providers
);

}   // namespace core::scheduler

template <typename ProviderTypeList_T, typename DependencyTypeList_T>
struct ProvidesDependencyHelper {
    template <size_t provider_index_T>
    struct type {
        constexpr static bool value =
            util::meta::any_of<DependencyTypeList_T>([]<typename Dependency_T> {
                return core::scheduler::provides_dependency_c<
                    util::meta::type_list_at_t<ProviderTypeList_T, provider_index_T>,
                    Dependency_T>;
            });
    };
};

template <core::scheduler::decays_to_task_c Task_T, typename... DependencyProviders_T>
constexpr auto core::scheduler::invoke_with_right_dependency_providers(
    Task_T&& task,
    DependencyProviders_T... dependency_providers
)
{
    using RightProviderIndices_T = util::meta::index_sequence_filter_t<
        std::make_index_sequence<sizeof...(DependencyProviders_T)>,
        ProvidesDependencyHelper<
            util::TypeList<DependencyProviders_T...>,
            typename std::remove_cvref_t<Task_T>::Dependencies>::template type>;

    return util::meta::apply<RightProviderIndices_T>([&dependency_providers...,
                                                      &task]<size_t... provider_indices_T> {
        return std::forward<Task_T>(task)(dependency_providers...[provider_indices_T]...);
    });
}
