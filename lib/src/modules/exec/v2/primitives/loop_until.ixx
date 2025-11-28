module;

#include <memory>
#include <optional>
#include <utility>

export module ddge.modules.exec.v2.primitives.loop_until;

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
import ddge.modules.exec.v2.TaskFinishedCallback;
import ddge.modules.exec.v2.TaskHubBuilder;
import ddge.modules.exec.v2.TaskHubProxy;
import ddge.modules.exec.v2.TypedTaskIndex;

namespace ddge::exec::v2 {

export template <convertible_to_TaskBlueprint_c<void> MainTaskBlueprint_T>
auto loop_until(
    MainTaskBlueprint_T&&                       main_blueprint,
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

template <ddge::exec::v2::convertible_to_TaskBlueprint_c<void> MainTaskBlueprint_T>
auto ddge::exec::v2::loop_until(
    MainTaskBlueprint_T&&                       main_blueprint,
    TaskBlueprint<bool, Cardinality::eSingle>&& predicate_blueprint
) -> TaskBlueprint<void, Cardinality::eSingle>
{
    return TaskBlueprint<void, Cardinality::eSingle>{
        [x_main_blueprint      = as_task_blueprint<void>(std::move(main_blueprint)),
         x_predicate_blueprint = std::move(predicate_blueprint)]   //
        () mutable -> TaskBuilder<void> {
            return TaskBuilder<void>{
                [y_main_blueprint      = std::move(x_main_blueprint),
                 y_predicate_blueprint = std::move(x_predicate_blueprint)](
                    TaskHubBuilder&              task_hub_builder,
                    TaskFinishedCallback<void>&& callback
                ) mutable -> TypedTaskIndex<void>   //
                {
                    class Looper {
                    public:
                        auto set_predicate_task_index(
                            const TypedTaskIndex<bool> predicate_task_index
                        ) -> void
                        {
                            m_predicate_task_index = predicate_task_index;
                        }
                        auto schedule_next_iteration(
                            const TaskHubProxy& task_hub_proxy
                        ) const -> void
                        {
                            task_hub_proxy.schedule(*m_predicate_task_index);
                        }

                    private:
                        std::optional<TypedTaskIndex<bool>> m_predicate_task_index;
                    };

                    std::shared_ptr<Looper> looper{ std::make_shared<Looper>() };
                    std::shared_ptr<std::optional<TaskContinuation<void>>>
                        shared_continuation{
                            std::make_shared<std::optional<TaskContinuation<void>>>()
                        };

                    const TypedTaskIndex<bool> predicate_task_index =
                        std::move(y_predicate_blueprint)
                            .materialize()
                            .build(
                                task_hub_builder,
                                TaskFinishedCallback<bool>{
                                    [x_callback = std::move(callback),
                                     main_task_index =
                                         ::sync(std::move(y_main_blueprint).materialize())
                                             .build(
                                                 task_hub_builder,
                                                 TaskFinishedCallback<void>{
                                                     [looper](
                                                         const TaskHubProxy& task_hub_proxy
                                                     ) {
                                                         looper->schedule_next_iteration(
                                                             task_hub_proxy
                                                         );
                                                     }   //
                                                 }
                                             ),
                                     shared_continuation]                         //
                                    (const TaskHubProxy& task_hub_proxy,
                                     const bool should_execute) mutable -> void   //
                                    {
                                        if (should_execute) {
                                            task_hub_proxy.schedule(main_task_index);
                                        }
                                        else {
                                            if (shared_continuation->has_value()) {
                                                (**shared_continuation)();
                                            }
                                            else {
                                                x_callback(task_hub_proxy);
                                            }
                                        }
                                    }   //
                                }
                            );

                    looper->set_predicate_task_index(std::move(predicate_task_index));

                    return task_hub_builder.emplace_indirect_task_factory(
                        IndirectTaskFactory<void>{
                            IndirectTaskBody{
                                [looper](const TaskHubProxy& task_hub_proxy) {
                                    looper->schedule_next_iteration(task_hub_proxy);
                                }   //
                            },
                            IndirectTaskContinuationSetter<void>{
                                [shared_continuation](
                                    TaskContinuation<void>&& continuation
                                ) mutable -> void {
                                    *shared_continuation = std::move(continuation);
                                }   //
                            }   //
                        }
                    );
                }   //
            };
        }   //
    };
}
