module;

#include <memory>
#include <optional>
#include <utility>

export module ddge.modules.scheduler.ScheduleBuilder;

import ddge.modules.scheduler.locks.LockGroup;
import ddge.modules.scheduler.as_task_blueprint;
import ddge.modules.scheduler.Cardinality;
import ddge.modules.scheduler.convertible_to_TaskBlueprint_c;
import ddge.modules.scheduler.gatherers.WaitAll;
import ddge.modules.scheduler.IndirectTaskContinuationSetter;
import ddge.modules.scheduler.IndirectTaskFactory;
import ddge.modules.scheduler.TaskBlueprint;
import ddge.modules.scheduler.TaskBuilder;
import ddge.modules.scheduler.TaskBuilderBundle;
import ddge.modules.scheduler.TaskContinuation;
import ddge.modules.scheduler.TaskContinuationFactory;
import ddge.modules.scheduler.TaskHubBuilder;
import ddge.modules.scheduler.TaskHubProxy;
import ddge.modules.scheduler.TypedTaskIndex;

namespace ddge::scheduler {

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

}   // namespace ddge::scheduler

ddge::scheduler::ScheduleBuilder::ScheduleBuilder(
    TaskBlueprint<void, Cardinality::eSingle>&& initial
)
    : m_task_blueprint{ std::move(initial) }
{}

ddge::scheduler::ScheduleBuilder::ScheduleBuilder(
    TaskBlueprint<void, Cardinality::eMulti>&& initial
)
    : m_task_blueprint{
          [x_initial = std::move(initial)] mutable -> TaskBuilder<void> {
              return std::move(x_initial).materialize().sync(WaitAllBuilder{});
          }   //
      }
{}

[[nodiscard]]
auto sync(ddge::scheduler::TaskBuilder<void>&& task_builder)
    -> ddge::scheduler::TaskBuilder<void>
{
    return std::move(task_builder);
}

[[nodiscard]]
auto sync(ddge::scheduler::TaskBuilderBundle<void>&& task_builder_bundle)
    -> ddge::scheduler::TaskBuilder<void>
{
    return std::move(task_builder_bundle).sync(ddge::scheduler::WaitAllBuilder{});
}

template <ddge::scheduler::convertible_to_TaskBlueprint_c<void> TaskBlueprint_T>
auto ddge::scheduler::ScheduleBuilder::then(TaskBlueprint_T&& next) && -> ScheduleBuilder
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

                        LockGroup locks;
                        locks.expand(task_hub_builder.locks_of(previous_task_index));
                        locks.expand(task_hub_builder.locks_of(next_task_index));

                        return task_hub_builder.emplace_indirect_task_factory(
                            IndirectTaskFactory<void>{
                                previous_task_index,
                                IndirectTaskContinuationSetter<void>{
                                    [shared_continuation](
                                        TaskContinuation<void>&& continuation
                                    ) -> void {
                                        (*shared_continuation) = std::move(continuation);
                                    }   //
                                },
                                std::move(locks)   //
                            }
                        );
                    }   //
                };
            }   //
        }   //
    };
}

ddge::scheduler::ScheduleBuilder::operator TaskBlueprint<void, Cardinality::eSingle>() &&
{
    return std::move(m_task_blueprint);
}
