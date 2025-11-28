module;

#include <memory>
#include <optional>
#include <utility>

export module ddge.modules.exec.v2.primitives.run_if;

import ddge.modules.exec.v2.Cardinality;
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

export template <Cardinality cardinality_T>
auto run_if(
    TaskBlueprint<void, cardinality_T>&&        main_blueprint,
    TaskBlueprint<bool, Cardinality::eSingle>&& predicate_blueprint
) -> TaskBlueprint<void, Cardinality::eSingle>;

}   // namespace ddge::exec::v2

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

template <ddge::exec::v2::Cardinality cardinality_T>
auto ddge::exec::v2::run_if(
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

                    return task_hub_builder.emplace_indirect_task_factory(
                        IndirectTaskFactory<void>{
                            IndirectTaskBody{
                                [predicate_task_index](const TaskHubProxy& task_hub_proxy) {
                                    task_hub_proxy.schedule(predicate_task_index);
                                } },
                            IndirectTaskContinuationSetter<void>{
                                [x_shared_continuation = std::move(shared_continuation)](
                                    TaskContinuation<void>&& continuation
                                ) mutable -> void {
                                    *x_shared_continuation = std::move(continuation);
                                }   //
                            }   //
                        }
                    );
                }   //
            };
        }   //
    };
}
