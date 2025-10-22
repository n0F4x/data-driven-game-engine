module;

#include <utility>

// TODO: remove once Clang can mangle
#include <function2/function2.hpp>

export module ddge.modules.execution.v2.ScheduleBuilder;

import ddge.modules.execution.Nexus;
import ddge.modules.execution.v2.as_task_blueprint;
import ddge.modules.execution.v2.Cardinality;
import ddge.modules.execution.v2.convertible_to_TaskBlueprint_c;
import ddge.modules.execution.v2.gatherers.WaitAll;
import ddge.modules.execution.v2.TaskBlueprint;
import ddge.modules.execution.v2.TaskBuilder;
import ddge.modules.execution.v2.TaskBuilderBundle;
import ddge.modules.execution.v2.TaskBundle;
import ddge.modules.execution.v2.TaskFinishedCallback;
import ddge.modules.execution.v2.TaskHubBuilder;
import ddge.modules.execution.v2.TaskHubProxy;

namespace ddge::exec::v2 {

export class ScheduleBuilder {
public:
    explicit ScheduleBuilder(TaskBlueprint<void, Cardinality::eSingle>&& initial);
    explicit ScheduleBuilder(TaskBlueprint<void, Cardinality::eMulti>&& initial);

    template <convertible_to_TaskBlueprint_c<void> TaskBlueprint_T>
    [[nodiscard]]
    auto then(TaskBlueprint_T&& next) && -> ScheduleBuilder;

    explicit(false) operator TaskBlueprint<void, Cardinality::eSingle>() &&;

private:
    TaskBlueprint<void, Cardinality::eSingle> m_task_blueprint;
};

}   // namespace ddge::exec::v2

ddge::exec::v2::ScheduleBuilder::ScheduleBuilder(
    TaskBlueprint<void, Cardinality::eSingle>&& initial
)
    : m_task_blueprint{ std::move(initial) }
{}

ddge::exec::v2::ScheduleBuilder::ScheduleBuilder(
    TaskBlueprint<void, Cardinality::eMulti>&& initial
)
    : m_task_blueprint{
          [x_initial = std::move(initial)] mutable -> TaskBuilder<void> {
              return std::move(x_initial).materialize().sync(WaitAllBuilder{});
          }   //
      }
{}

[[nodiscard]]
auto sync(ddge::exec::v2::TaskBuilder<void>&& task_builder)
    -> ddge::exec::v2::TaskBuilder<void>
{
    return std::move(task_builder);
}

[[nodiscard]]
auto sync(ddge::exec::v2::TaskBuilderBundle<void>&& task_builder_bundle)
    -> ddge::exec::v2::TaskBuilder<void>
{
    return std::move(task_builder_bundle).sync(ddge::exec::v2::WaitAllBuilder{});
}

template <ddge::exec::v2::convertible_to_TaskBlueprint_c<void> TaskBlueprint_T>
auto ddge::exec::v2::ScheduleBuilder::then(TaskBlueprint_T&& next) && -> ScheduleBuilder
{
    return ScheduleBuilder{
        TaskBlueprint<void, Cardinality::eSingle>{
            [previous = std::move(m_task_blueprint),
             x_next   = as_task_blueprint<void>(std::move(next))]   //
            () mutable -> TaskBuilder<void>                       //
            {
                return TaskBuilder<void>{
                    [x_previous = std::move(previous),
                     y_next     = std::move(x_next)]   //
                    (Nexus & nexus,
                     TaskHubBuilder & task_hub_builder,
                     TaskFinishedCallback<void>&& callback) mutable -> TaskBundle   //
                    {
                        return std::move(x_previous)
                            .materialize()
                            .build(
                                nexus,
                                task_hub_builder,
                                [next_task =
                                     ::sync(std::move(y_next).materialize())
                                         .build(
                                             nexus, task_hub_builder, std::move(callback)
                                         )]                                            //
                                (const TaskHubProxy& task_hub_proxy) mutable -> void   //
                                {                                                      //
                                    next_task(task_hub_proxy);
                                }
                            );
                    }   //
                };
            }   //
        }   //
    };
}

ddge::exec::v2::ScheduleBuilder::operator TaskBlueprint<void, Cardinality::eSingle>() &&
{
    return std::move(m_task_blueprint);
}
