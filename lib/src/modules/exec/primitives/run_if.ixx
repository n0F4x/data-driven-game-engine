module;

#include <memory>
#include <optional>
#include <utility>

export module ddge.modules.exec.primitives.run_if;

import ddge.modules.exec.locks.LockGroup;
import ddge.modules.exec.Cardinality;
import ddge.modules.exec.gatherers.WaitAll;
import ddge.modules.exec.IndirectTaskContinuationSetter;
import ddge.modules.exec.IndirectTaskFactory;
import ddge.modules.exec.TaskBlueprint;
import ddge.modules.exec.TaskBuilder;
import ddge.modules.exec.TaskBuilderBundle;
import ddge.modules.exec.TaskContinuation;
import ddge.modules.exec.TaskContinuationFactory;
import ddge.modules.exec.TaskHubBuilder;
import ddge.modules.exec.TaskHubProxy;
import ddge.modules.exec.TypedTaskIndex;

namespace ddge::exec {

export template <Cardinality cardinality_T>
auto run_if(
    TaskBlueprint<void, cardinality_T>&&        main_blueprint,
    TaskBlueprint<bool, Cardinality::eSingle>&& predicate_blueprint
) -> TaskBlueprint<void, Cardinality::eSingle>;

}   // namespace ddge::exec

[[nodiscard]]
auto sync(ddge::exec::TaskBuilder<void>&& task_builder) -> ddge::exec::TaskBuilder<void>
{
    return std::move(task_builder);
}

[[nodiscard]]
auto sync(ddge::exec::TaskBuilderBundle<void>&& task_builder_bundle)
    -> ddge::exec::TaskBuilder<void>
{
    return std::move(task_builder_bundle).sync(ddge::exec::WaitAllBuilder{});
}

template <ddge::exec::Cardinality cardinality_T>
auto ddge::exec::run_if(
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
