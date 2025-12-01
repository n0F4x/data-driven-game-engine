module;

#include <concepts>
#include <memory>
#include <optional>
#include <utility>

export module ddge.modules.scheduler.primitives.repeat;

import ddge.modules.scheduler.locks.LockGroup;
import ddge.modules.scheduler.IndirectTaskContinuationSetter;
import ddge.modules.scheduler.IndirectTaskFactory;
import ddge.modules.scheduler.TaskBuilder;
import ddge.modules.scheduler.TaskContinuation;
import ddge.modules.scheduler.TaskContinuationFactory;
import ddge.modules.scheduler.TaskHubBuilder;
import ddge.modules.scheduler.TaskHubProxy;
import ddge.modules.scheduler.TypedTaskFactoryHandle;
import ddge.modules.scheduler.TypedTaskIndex;

namespace ddge::scheduler {

export template <std::integral Repetition_T>
[[nodiscard]]
auto repeat(
    TaskBuilder<void>&&         body_builder,
    TaskBuilder<Repetition_T>&& repetition_specifier_builder
) -> TaskBuilder<void>;

}   // namespace ddge::scheduler

template <std::integral Repetition_T>
auto ddge::scheduler::repeat(
    TaskBuilder<void>&&         body_builder,
    TaskBuilder<Repetition_T>&& repetition_specifier_builder
) -> TaskBuilder<void>
{
    return TaskBuilder<void>{
        [x_body_builder                 = std::move(body_builder),
         x_repetition_specifier_builder = std::move(repetition_specifier_builder)](
            TaskHubBuilder& task_hub_builder
        ) mutable -> TypedTaskFactoryHandle<void>   //
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
                auto schedule_next_iteration(const TaskHubProxy& task_hub_proxy) -> void
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

            const TypedTaskFactoryHandle<void> body_task_handle =   //
                std::move(x_body_builder).build(task_hub_builder);

            std::shared_ptr<Looper> looper{
                std::make_shared<Looper>(body_task_handle.index())   //
            };

            body_task_handle->set_continuation_factory(
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

            const TypedTaskFactoryHandle<Repetition_T> repetition_specifier_task_handle =
                std::move(x_repetition_specifier_builder).build(task_hub_builder);

            repetition_specifier_task_handle->set_continuation_factory(
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
            locks.expand(body_task_handle->locks());
            locks.expand(repetition_specifier_task_handle->locks());

            return task_hub_builder.emplace_indirect_task_factory(
                IndirectTaskFactory<void>{
                    repetition_specifier_task_handle.index(),
                    IndirectTaskContinuationSetter<void>{
                        [looper](TaskContinuation<void>&& continuation) mutable -> void {
                            looper->set_continuation(std::move(continuation));
                        }   //
                    },
                    std::move(locks)   //
                }
            );
        }
    };
}
