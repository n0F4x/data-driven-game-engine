module;

#include <ranges>
#include <utility>
#include <vector>

export module ddge.modules.scheduler.ScheduleBuilder;

import ddge.modules.scheduler.locks.LockGroup;
import ddge.modules.scheduler.IndirectTaskContinuationSetter;
import ddge.modules.scheduler.IndirectTaskFactory;
import ddge.modules.scheduler.TaskBuilder;
import ddge.modules.scheduler.TaskContinuation;
import ddge.modules.scheduler.TaskContinuationFactory;
import ddge.modules.scheduler.TaskHubBuilder;
import ddge.modules.scheduler.TaskHubProxy;
import ddge.modules.scheduler.TaskResultTrait;
import ddge.modules.scheduler.TypedTaskFactoryHandle;

namespace ddge::scheduler {

export class ScheduleBuilder {
public:
    explicit ScheduleBuilder(TaskBuilder<void>&& first);

    [[nodiscard]]
    auto then(TaskBuilder<void>&& next) && -> ScheduleBuilder;

    [[nodiscard]]
    explicit(false) operator TaskBuilder<void>() &&;

private:
    std::vector<TaskBuilder<void>> m_task_builders;
};

export template <>
struct TaskResultTrait<ScheduleBuilder> {
    using type = void;
};

}   // namespace ddge::scheduler

ddge::scheduler::ScheduleBuilder::ScheduleBuilder(TaskBuilder<void>&& first)
{
    m_task_builders.emplace_back(std::move(first));
}

auto ddge::scheduler::ScheduleBuilder::then(TaskBuilder<void>&& next) && -> ScheduleBuilder
{
    m_task_builders.emplace_back(std::move(next));

    return std::move(*this);
}

ddge::scheduler::ScheduleBuilder::operator TaskBuilder<void>() &&
{
    return TaskBuilder<void>{
        [task_builders = std::move(m_task_builders)](
            TaskHubBuilder& task_hub_builder
        ) mutable -> TypedTaskFactoryHandle<void>   //
        {
            std::vector<TypedTaskFactoryHandle<void>> task_handles{
                std::from_range,
                std::move(task_builders) | std::views::as_rvalue
                    | std::views::transform(
                        [&task_hub_builder](TaskBuilder<void>&& builder)
                            -> TypedTaskFactoryHandle<void>   //
                        {                                     //
                            return std::move(builder).build(task_hub_builder);
                        }
                    )
            };

            // TODO: use std::views::adjacent
            for (const std::size_t previous_task_handle_index :
                 std::views::iota(0uz, task_handles.size() - 1))
            {
                task_handles[previous_task_handle_index]->set_continuation_factory(
                    TaskContinuationFactory<void>{
                        [next_task_index =
                             task_handles[previous_task_handle_index + 1].index()](
                            const TaskHubProxy task_hub_proxy
                        ) -> TaskContinuation<void> {
                            return TaskContinuation<void>{
                                [next_task_index, task_hub_proxy] -> void {
                                    task_hub_proxy.schedule(next_task_index);
                                }   //
                            };
                        }   //
                    }
                );
            }

            LockGroup locks;
            for (const TypedTaskFactoryHandle<void> task_handle : task_handles) {
                locks.expand(task_handle->locks());
            }

            return task_hub_builder.emplace_indirect_task_factory(
                IndirectTaskFactory<void>{
                    task_handles.front().index(),
                    IndirectTaskContinuationSetter<void>{
                        [last_task_handle = task_handles.back()](
                            TaskContinuation<void>&& continuation
                        ) -> void {
                            last_task_handle->set_continuation_factory(
                                TaskContinuationFactory<void>{
                                    [x_continuation = std::move(continuation)](
                                        const TaskHubProxy
                                    ) mutable -> TaskContinuation<void> {
                                        return std::move(x_continuation);
                                    }   //
                                }
                            );
                        }   //
                    },
                    std::move(locks)   //
                }
            );
        }   //
    };
}
