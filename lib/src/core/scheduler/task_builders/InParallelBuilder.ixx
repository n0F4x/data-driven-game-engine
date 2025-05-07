module;

#include <concepts>
#include <tuple>
#include <utility>

export module core.scheduler.task_builders.InParallelBuilder;

import core.scheduler.build;
import core.scheduler.concepts.task_builder_c;
import core.scheduler.task_builders.TaskBuilderBase;

import utility.meta.algorithms.apply;
import utility.meta.algorithms.for_each;
import utility.meta.type_traits.type_list.type_list_concat;
import utility.meta.type_traits.type_list.type_list_unique;

namespace core::scheduler {

export template <task_builder_c... TaskBuilders_T>
class InParallelBuilder : public TaskBuilderBase {
public:
    using Result          = void;
    using UniqueArguments = util::meta::type_list_unique_t<
        util::meta::type_list_concat_t<typename TaskBuilders_T::UniqueArguments...>>;

    template <typename... UTaskBuilders_T>
        requires(std::constructible_from<TaskBuilders_T, UTaskBuilders_T &&> && ...)
    constexpr explicit InParallelBuilder(UTaskBuilders_T&&... task_builders);

    template <typename Self_T, typename... ArgumentProviders_T>
    [[nodiscard]]
    constexpr auto operator()(this Self_T&&, ArgumentProviders_T&&... argument_providers);

private:
    std::tuple<TaskBuilders_T...> m_task_builders;
};

}   // namespace core::scheduler

template <core::scheduler::task_builder_c... TaskBuilders_T>
template <typename... UTaskBuilders_T>
    requires(std::constructible_from<TaskBuilders_T, UTaskBuilders_T &&> && ...)
constexpr core::scheduler::InParallelBuilder<TaskBuilders_T...>::InParallelBuilder(
    UTaskBuilders_T&&... task_builders
)
    : m_task_builders{ std::forward<UTaskBuilders_T>(task_builders)... }
{}

template <core::scheduler::task_builder_c... TaskBuilders_T>
template <typename Self_T, typename... ArgumentProviders_T>
constexpr auto core::scheduler::InParallelBuilder<TaskBuilders_T...>::operator()(
    this Self_T&& self,
    ArgumentProviders_T&&... argument_providers
)
{
    const auto build_tasks = [&self, &argument_providers...] {
        return util::meta::apply<std::make_index_sequence<sizeof...(TaskBuilders_T)>>(
            [&self, &argument_providers...]<size_t... task_builder_indices_T> {
                return std::make_tuple(build(
                    std::get<task_builder_indices_T>(self.m_task_builders),
                    argument_providers...
                )...);
            }
        );
    };

    return [tasks = build_tasks()] mutable {
        util::meta::for_each<std::make_index_sequence<sizeof...(TaskBuilders_T)>>(
            [&tasks]<size_t task_index_T> { std::invoke(std::get<task_index_T>(tasks)); }
        );
    };
}
