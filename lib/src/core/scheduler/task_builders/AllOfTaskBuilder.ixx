module;

#include <concepts>
#include <tuple>
#include <utility>

export module core.scheduler.task_builders.AllOfTaskBuilder;

import core.scheduler.build;
import core.scheduler.concepts.predicate_task_builder_c;
import core.scheduler.task_builders.TaskBuilderBase;

import utility.meta.algorithms.all_of;
import utility.meta.algorithms.apply;
import utility.meta.type_traits.type_list.type_list_union;

namespace core::scheduler {

export template <predicate_task_builder_c... PredicateTaskBuilders_T>
class AllOfTaskBuilder : public TaskBuilderBase {
public:
    using Result = bool;
    using UniqueArguments =
        util::meta::type_list_union_t<typename PredicateTaskBuilders_T::UniqueArguments...>;

    template <typename... UPredicateTaskBuilders_T>
        requires(
            std::constructible_from<PredicateTaskBuilders_T, UPredicateTaskBuilders_T &&>
            && ...
        )
    constexpr explicit AllOfTaskBuilder(
        UPredicateTaskBuilders_T&&... predicate_task_builders
    );

    template <typename Self_T, typename... ArgumentProviders_T>
    [[nodiscard]]
    constexpr auto operator()(this Self_T&&, ArgumentProviders_T&&... argument_providers);

private:
    std::tuple<PredicateTaskBuilders_T...> m_predicate_task_builders;
};

}   // namespace core::scheduler

template <core::scheduler::predicate_task_builder_c... PredicateTaskBuilders_T>
template <typename... UPredicateTaskBuilders_T>
    requires(
        std::constructible_from<PredicateTaskBuilders_T, UPredicateTaskBuilders_T &&>
        && ...
    )
constexpr core::scheduler::AllOfTaskBuilder<PredicateTaskBuilders_T...>::AllOfTaskBuilder(
    UPredicateTaskBuilders_T&&... predicate_task_builders
)
    : m_predicate_task_builders{
          std::forward<UPredicateTaskBuilders_T>(predicate_task_builders)...
      }
{}

template <core::scheduler::predicate_task_builder_c... PredicateTaskBuilders_T>
template <typename Self_T, typename... ArgumentProviders_T>
constexpr auto core::scheduler::AllOfTaskBuilder<PredicateTaskBuilders_T...>::operator()(
    this Self_T&& self,
    ArgumentProviders_T&&... argument_providers
)
{
    const auto build_tasks = [&self, &argument_providers...] {
        return util::meta::
            apply<std::make_index_sequence<sizeof...(PredicateTaskBuilders_T)>>(
                [&self, &argument_providers...]<std::size_t... task_builder_indices_T> {
                    return std::make_tuple(build(
                        std::get<task_builder_indices_T>(self.m_predicate_task_builders),
                        argument_providers...
                    )...);
                }
            );
    };

    return [tasks = build_tasks()] mutable -> Result {
        return util::meta::
            all_of<std::make_index_sequence<sizeof...(PredicateTaskBuilders_T)>>(
                [&tasks]<std::size_t task_index_T> {
                    return std::invoke(std::get<task_index_T>(tasks));
                }
            );
    };
}
