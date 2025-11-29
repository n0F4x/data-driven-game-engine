module;

#include <concepts>
#include <memory>
#include <optional>
#include <utility>

export module ddge.modules.exec.v2.primitives.repeat;

import ddge.modules.exec.locks.LockGroup;
import ddge.modules.exec.v2.as_task_blueprint;
import ddge.modules.exec.v2.Cardinality;
import ddge.modules.exec.v2.convertible_to_TaskBlueprint_c;
import ddge.modules.exec.v2.gatherers.WaitAll;
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

export template <
    convertible_to_TaskBlueprint_c<void> MainTaskBlueprint_T,
    std::integral                        Repetition_T>
auto repeat(
    MainTaskBlueprint_T&&                               main_blueprint,
    TaskBlueprint<Repetition_T, Cardinality::eSingle>&& repetition_specifier_blueprint
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

template <
    ddge::exec::v2::convertible_to_TaskBlueprint_c<void> MainTaskBlueprint_T,
    std::integral                                        Repetition_T>
auto ddge::exec::v2::repeat(
    MainTaskBlueprint_T&&                               main_blueprint,
    TaskBlueprint<Repetition_T, Cardinality::eSingle>&& repetition_specifier_blueprint
) -> TaskBlueprint<void, Cardinality::eSingle>
{
    return TaskBlueprint<void, Cardinality::eSingle>{
        [x_main_blueprint = as_task_blueprint<void>(std::move(main_blueprint)),
         x_repetition_specifier_blueprint = std::move(repetition_specifier_blueprint)]   //
        () mutable -> TaskBuilder<void> {
            return TaskBuilder<void>{
                [y_main_blueprint = std::move(x_main_blueprint),
                 y_repetition_specifier_blueprint =
                     std::move(x_repetition_specifier_blueprint)](   //
                    TaskHubBuilder& task_hub_builder
                ) mutable -> TypedTaskIndex<void>                    //
                {
                    class Looper {
                    public:
                        explicit Looper(const TypedTaskIndex<void> looped_task_index)
                            : m_looped_task_index{ looped_task_index }
                        {}
                        auto set_continuation(TaskContinuation<void>&& continuation)
                        {
                            m_continuation = std::move(continuation);
                        }
                        auto set_repetition(Repetition_T repetition) -> void
                        {
                            m_repetition = repetition;
                        }
                        auto schedule_next_iteration(const TaskHubProxy& task_hub_proxy)
                            -> void
                        {
                            if (m_repetition > 0) {
                                --*m_repetition;
                                task_hub_proxy.schedule(*m_looped_task_index);
                            }
                            else {
                                if (m_continuation.has_value()) {
                                    (*m_continuation)();
                                }
                            }
                        }

                    private:
                        std::optional<Repetition_T>           m_repetition;
                        std::optional<TypedTaskIndex<void>>   m_looped_task_index;
                        std::optional<TaskContinuation<void>> m_continuation;
                    };

                    const TypedTaskIndex<void> main_task_index =   //
                        ::sync(std::move(y_main_blueprint).materialize())
                            .build(task_hub_builder);

                    std::shared_ptr<Looper> looper{
                        std::make_shared<Looper>(main_task_index)   //
                    };

                    task_hub_builder.set_task_continuation_factory(
                        main_task_index,
                        TaskContinuationFactory<void>{
                            [looper](const TaskHubProxy task_hub_proxy) mutable
                                -> TaskContinuation<void> {
                                return TaskContinuation<void>{
                                    [looper, task_hub_proxy] mutable -> void {
                                        looper->schedule_next_iteration(task_hub_proxy);
                                    }   //
                                };
                            }   //
                        }
                    );

                    const TypedTaskIndex<Repetition_T> repetition_specifier_task_index =   //
                        std::move(y_repetition_specifier_blueprint)
                            .materialize()
                            .build(task_hub_builder);

                    task_hub_builder.set_task_continuation_factory(
                        repetition_specifier_task_index,
                        TaskContinuationFactory<Repetition_T>{
                            [looper](const TaskHubProxy task_hub_proxy) mutable
                                -> TaskContinuation<Repetition_T> {
                                return TaskContinuation<Repetition_T>{
                                    [looper, task_hub_proxy](
                                        const Repetition_T repetition
                                    ) mutable -> void {
                                        looper->set_repetition(repetition);
                                        looper->schedule_next_iteration(task_hub_proxy);
                                    }   //
                                };
                            }   //
                        }
                    );

                    LockGroup locks;
                    locks.expand(task_hub_builder.locks_of(main_task_index));
                    locks.expand(
                        task_hub_builder.locks_of(repetition_specifier_task_index)
                    );

                    return task_hub_builder.emplace_indirect_task_factory(
                        IndirectTaskFactory<void>{
                            repetition_specifier_task_index,
                            IndirectTaskContinuationSetter<void>{
                                [looper](TaskContinuation<void>&& continuation) mutable
                                    -> void {
                                    looper->set_continuation(std::move(continuation));
                                }   //
                            },
                            std::move(locks)   //
                        }
                    );
                }
            };
        }   //
    };
}
