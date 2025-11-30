module;

#include <memory>
#include <optional>
#include <utility>

export module ddge.modules.scheduler.primitives.run_if;

import ddge.modules.scheduler.locks.LockGroup;
import ddge.modules.scheduler.Cardinality;
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

export template <Cardinality cardinality_T>
auto run_if(
    TaskBlueprint<void, cardinality_T>&&        main_blueprint,
    TaskBlueprint<bool, Cardinality::eSingle>&& predicate_blueprint
) -> TaskBlueprint<void, Cardinality::eSingle>;

}   // namespace ddge::scheduler

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

template <ddge::scheduler::Cardinality cardinality_T>
auto ddge::scheduler::run_if(
    TaskBlueprint<void, cardinality_T>&&        main_blueprint,
    TaskBlueprint<bool, Cardinality::eSingle>&& predicate_blueprint
) -> TaskBlueprint<void, Cardinality::eSingle>
{
    return TaskBlueprint<void, Cardinality::eSingle>{
        [x_main_blueprint      = std::move(main_blueprint),
         x_predicate_blueprint = std::move(predicate_blueprint)]   //
        () mutable -> TaskBuilder<void>                            //
        {
            return TaskBuilder<void>{
                [y_main_blueprint      = std::move(x_main_blueprint),
                 y_predicate_blueprint = std::move(x_predicate_blueprint)](
                    TaskHubBuilder& task_hub_builder
                ) mutable -> TypedTaskIndex<void>   //
                {
                    const TypedTaskIndex<void> main_task_index =
                        ::sync(std::move(y_main_blueprint).materialize())
                            .build(task_hub_builder);

                    std::shared_ptr<std::optional<TaskContinuation<void>>>
                        shared_continuation{
                            std::make_shared<std::optional<TaskContinuation<void>>>()
                        };

                    task_hub_builder.set_task_continuation_factory(
                        main_task_index,
                        TaskContinuationFactory<void>{
                            [shared_continuation](const TaskHubProxy) mutable
                                -> TaskContinuation<void> {
                                return TaskContinuation<void>{
                                    [shared_continuation] mutable -> void {
                                        if (shared_continuation->has_value()) {
                                            (**shared_continuation)();
                                        }
                                    }   //
                                };
                            }   //
                        }
                    );

                    const TypedTaskIndex<bool> predicate_task_index =
                        std::move(y_predicate_blueprint)
                            .materialize()
                            .build(task_hub_builder);

                    task_hub_builder.set_task_continuation_factory(
                        predicate_task_index,
                        TaskContinuationFactory<bool>{
                            [main_task_index, shared_continuation](
                                const TaskHubProxy task_hub_proxy
                            ) mutable -> TaskContinuation<bool> {
                                return TaskContinuation<bool>{
                                    [main_task_index, shared_continuation, task_hub_proxy](
                                        const bool should_execute
                                    ) mutable -> void {
                                        if (should_execute) {
                                            task_hub_proxy.schedule(main_task_index);
                                        }
                                        else {
                                            if (shared_continuation->has_value()) {
                                                (**shared_continuation)();
                                            }
                                        }
                                    }   //
                                };
                            }   //
                        }
                    );

                    LockGroup locks;
                    locks.expand(task_hub_builder.locks_of(main_task_index));
                    locks.expand(task_hub_builder.locks_of(predicate_task_index));

                    return task_hub_builder.emplace_indirect_task_factory(
                        IndirectTaskFactory<void>{
                            predicate_task_index,
                            IndirectTaskContinuationSetter<void>{
                                [x_shared_continuation = std::move(shared_continuation)](
                                    TaskContinuation<void>&& continuation
                                ) mutable -> void {
                                    *x_shared_continuation = std::move(continuation);
                                }   //
                            },
                            std::move(locks) }
                    );
                }   //
            };
        }   //
    };
}
