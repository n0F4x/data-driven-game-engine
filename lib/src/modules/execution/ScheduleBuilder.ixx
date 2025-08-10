module;

#include <concepts>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

export module ddge.modules.execution.SchedulerBuilder;

import ddge.modules.execution.Nexus;
import ddge.modules.execution.provide_accessors_for;
import ddge.modules.execution.raw_task_c;
import ddge.modules.execution.Task;
import ddge.modules.execution.TaskBuilder;

import ddge.utility.meta.type_traits.functional.result_of;

namespace ddge::exec {

export class SchedulerBuilder {
public:
    template <typename TaskBuilder_T>
        requires specialization_of_TaskBuilder_c<std::remove_cvref_t<TaskBuilder_T>>
              && std::same_as<typename std::remove_cvref_t<TaskBuilder_T>::Result, void>
    explicit SchedulerBuilder(TaskBuilder_T&& task_builder);

    template <typename Self_T>
    explicit(false) operator TaskBuilder<void>(this Self_T&&);

    template <typename Self_T, raw_task_c F>
        requires std::same_as<util::meta::result_of_t<F>, void>
    [[nodiscard]]
    auto then(this Self_T&&, F&& func) -> SchedulerBuilder;

    template <typename Self_T, typename TaskBuilder_T>
        requires specialization_of_TaskBuilder_c<std::remove_cvref_t<TaskBuilder_T>>
              && std::same_as<typename std::remove_cvref_t<TaskBuilder_T>::Result, void>
    [[nodiscard]]
    auto then(this Self_T&&, TaskBuilder_T&& task_builder) -> SchedulerBuilder;

    template <typename Self_T, typename ScheduleBuilder_T>
        requires std::same_as<std::remove_cvref_t<ScheduleBuilder_T>, SchedulerBuilder>
    [[nodiscard]]
    auto then(this Self_T&&, ScheduleBuilder_T&& schedule_builder) -> SchedulerBuilder;

    [[nodiscard]]
    auto build(Nexus& nexus) const -> Task<void>;

private:
    TaskBuilder<void> m_builder;
};

}   // namespace ddge::exec

template <typename TaskBuilder_T>
    requires ddge::exec::
                 specialization_of_TaskBuilder_c<std::remove_cvref_t<TaskBuilder_T>>
          && std::same_as<typename std::remove_cvref_t<TaskBuilder_T>::Result, void>
ddge::exec::SchedulerBuilder::SchedulerBuilder(TaskBuilder_T&& task_builder)
    : m_builder{ std::forward<TaskBuilder_T>(task_builder) }
{}

template <typename Self_T>
ddge::exec::SchedulerBuilder::operator TaskBuilder<void>(this Self_T && self)
{
    return std::forward_like<Self_T>(self.m_builder);
}

template <typename Self_T, ddge::exec::raw_task_c F>
    requires std::same_as<ddge::util::meta::result_of_t<F>, void>
auto ddge::exec::SchedulerBuilder::then(this Self_T&& self, F&& func)
    -> SchedulerBuilder
{
    return std::forward<Self_T>(self).then(
        TaskBuilder<util::meta::result_of_t<F>>{ [wrapped_func = std::forward<F>(func)](
                                                     Nexus& nexus
                                                 ) -> Task<util::meta::result_of_t<F>> {
            return Task<util::meta::result_of_t<F>>{
                [wrapped_func, accessors_tuple = provide_accessors_for<F>(nexus)] mutable
                    -> util::meta::result_of_t<F> {
                    return std::apply(wrapped_func, accessors_tuple);
                }
            };
        } }
    );
}

template <typename Self_T, typename TaskBuilder_T>
    requires ddge::exec::
                 specialization_of_TaskBuilder_c<std::remove_cvref_t<TaskBuilder_T>>
          && std::same_as<typename std::remove_cvref_t<TaskBuilder_T>::Result, void>
auto ddge::exec::SchedulerBuilder::then(
    this Self_T&&   self,
    TaskBuilder_T&& task_builder
) -> SchedulerBuilder
{
    return SchedulerBuilder{ TaskBuilder<void>{
        [wrapped_task_builder      = std::forward_like<Self_T>(self.m_builder),
         wrapped_next_task_builder = std::forward<TaskBuilder_T>(task_builder
         )](Nexus& nexus) -> Task<void> {
            return [task      = wrapped_task_builder.build(nexus),
                    next_task = wrapped_next_task_builder.build(nexus)] mutable -> void {
                task();
                std::invoke(next_task);
            };
        } } };
}

template <typename Self_T, typename ScheduleBuilder_T>
    requires std::
        same_as<std::remove_cvref_t<ScheduleBuilder_T>, ddge::exec::SchedulerBuilder>
    auto ddge::exec::SchedulerBuilder::then(
        this Self_T&&       self,
        ScheduleBuilder_T&& schedule_builder
    ) -> SchedulerBuilder
{
    return std::forward<Self_T>(self).then(
        std::forward<ScheduleBuilder_T>(schedule_builder).operator TaskBuilder<void>()
    );
}

auto ddge::exec::SchedulerBuilder::build(Nexus& nexus) const -> Task<void>
{
    return m_builder.build(nexus);
}
