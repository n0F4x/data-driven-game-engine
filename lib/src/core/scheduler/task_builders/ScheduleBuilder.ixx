module;

#include <concepts>
#include <tuple>
#include <utility>

export module core.scheduler.task_builders.ScheduleBuilder;

import core.scheduler.as_task_builder;
import core.scheduler.build;
import core.scheduler.concepts.converts_to_task_builder_c;
import core.scheduler.concepts.task_builder_c;
import core.scheduler.task_builders.TaskBuilderBase;

import utility.meta.algorithms.apply;
import utility.meta.algorithms.for_each;
import utility.meta.type_traits.type_list.type_list_union;

namespace core::scheduler {

export template <task_builder_c... TaskBuilders_T>
class ScheduleBuilder : public TaskBuilderBase {
public:
    using Result = void;
    using UniqueArguments =
        util::meta::type_list_union_t<typename TaskBuilders_T::UniqueArguments...>;

    template <typename... UTaskBuilders_T>
        requires(std::constructible_from<TaskBuilders_T, UTaskBuilders_T &&> && ...)
    constexpr explicit ScheduleBuilder(UTaskBuilders_T&&... task_builders);

    template <converts_to_task_builder_c TaskBuilder_T, typename Self_T>
    constexpr auto then(this Self_T&&, TaskBuilder_T&& task_builder)
        -> ScheduleBuilder<TaskBuilders_T..., as_task_builder_t<TaskBuilder_T>>;

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
constexpr core::scheduler::ScheduleBuilder<TaskBuilders_T...>::ScheduleBuilder(
    UTaskBuilders_T&&... task_builders
)
    : m_task_builders{ std::forward<UTaskBuilders_T>(task_builders)... }
{}

template <core::scheduler::task_builder_c... TaskBuilders_T>
template <core::scheduler::converts_to_task_builder_c TaskBuilder_T, typename Self_T>
constexpr auto core::scheduler::ScheduleBuilder<TaskBuilders_T...>::then(
    this Self_T&&   self,
    TaskBuilder_T&& task_builder
) -> ScheduleBuilder<TaskBuilders_T..., as_task_builder_t<TaskBuilder_T>>
{
    return std::apply(
        [&task_builder]<typename... XTaskBuilders_T>(XTaskBuilders_T&&... task_builders) {
            return ScheduleBuilder<TaskBuilders_T..., as_task_builder_t<TaskBuilder_T>>{
                std::forward<XTaskBuilders_T>(task_builders)...,
                std::forward<TaskBuilder_T>(task_builder)
            };
        },
        std::forward_like<Self_T>(self.m_task_builders)
    );
}

template <core::scheduler::task_builder_c... TaskBuilders_T>
template <typename Self_T, typename... ArgumentProviders_T>
constexpr auto core::scheduler::ScheduleBuilder<TaskBuilders_T...>::operator()(
    this Self_T&& self,
    ArgumentProviders_T&&... argument_providers
)
{
    const auto build_tasks = [&self, &argument_providers...] {
        return util::meta::apply<std::make_index_sequence<sizeof...(TaskBuilders_T)>>(
            [&self, &argument_providers...]<std::size_t... task_builder_indices_T> {
                return std::make_tuple(build(
                    std::get<task_builder_indices_T>(self.m_task_builders),
                    argument_providers...
                )...);
            }
        );
    };

    return [tasks = build_tasks()] mutable -> Result {
        util::meta::for_each<std::make_index_sequence<sizeof...(TaskBuilders_T)>>(
            [&tasks]<std::size_t task_index_T> { std::invoke(std::get<task_index_T>(tasks)); }
        );
    };
}
