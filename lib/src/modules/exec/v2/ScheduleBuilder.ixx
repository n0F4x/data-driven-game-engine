module;

#include <memory>
#include <optional>
#include <utility>

export module ddge.modules.exec.v2.ScheduleBuilder;

import ddge.modules.exec.v2.as_task_blueprint;
import ddge.modules.exec.v2.Cardinality;
import ddge.modules.exec.v2.convertible_to_TaskBlueprint_c;
import ddge.modules.exec.v2.gatherers.WaitAll;
import ddge.modules.exec.v2.IndirectTaskBody;
import ddge.modules.exec.v2.IndirectTaskContinuationSetter;
import ddge.modules.exec.v2.IndirectTaskFactory;
import ddge.modules.exec.v2.TaskBlueprint;
import ddge.modules.exec.v2.TaskBuilder;
import ddge.modules.exec.v2.TaskBuilderBundle;
import ddge.modules.exec.v2.TaskContinuation;
import ddge.modules.exec.v2.TaskContinuationFactory;
import ddge.modules.exec.v2.TaskHubBuilder;
import ddge.modules.exec.v2.TaskHubProxy;
import ddge.modules.exec.v2.TypedTaskIndex;

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
                     y_next     = std::move(x_next)]        //
                    (                                   //
                        TaskHubBuilder & task_hub_builder
                    ) mutable -> TypedTaskIndex<void>   //
                    {
                        const TypedTaskIndex<void> previous_task_index =
                            std::move(x_previous).materialize().build(task_hub_builder);

                        const TypedTaskIndex<void> next_task_index =
                            ::sync(std::move(y_next).materialize()).build(task_hub_builder);

                        task_hub_builder.set_task_continuation_factory(
                            previous_task_index,
                            TaskContinuationFactory<void>{
                                [next_task_index](const TaskHubProxy task_hub_proxy)
                                    -> TaskContinuation<void> {
                                    return TaskContinuation<void>{
                                        [next_task_index, task_hub_proxy] -> void {
                                            task_hub_proxy.schedule(next_task_index);
                                        }
                                    };
                                }   //
                            }
                        );

                        std::shared_ptr<std::optional<TaskContinuation<void>>>
                            shared_continuation{
                                std::make_shared<std::optional<TaskContinuation<void>>>()
                            };

                        task_hub_builder.set_task_continuation_factory(
                            next_task_index,
                            TaskContinuationFactory<void>{
                                [shared_continuation](const TaskHubProxy) mutable
                                    -> TaskContinuation<void> {
                                    return TaskContinuation<void>{
                                        [shared_continuation] mutable -> void {
                                            if (shared_continuation->has_value()) {
                                                (**shared_continuation)();
                                            }
                                        }
                                    };
                                }   //
                            }
                        );

                        return task_hub_builder.emplace_indirect_task_factory(
                            IndirectTaskFactory<void>{
                                IndirectTaskBody{
                                    [previous_task_index](
                                        const TaskHubProxy& task_hub_proxy
                                    ) -> void {
                                        task_hub_proxy.schedule(previous_task_index);
                                    }   //
                                },
                                IndirectTaskContinuationSetter<void>{
                                    [shared_continuation](
                                        TaskContinuation<void>&& continuation
                                    ) -> void {
                                        (*shared_continuation) = std::move(continuation);
                                    }   //
                                }   //
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
