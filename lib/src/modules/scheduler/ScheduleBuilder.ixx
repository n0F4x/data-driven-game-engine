module;

#include <memory>
#include <optional>
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
import ddge.modules.scheduler.TypedTaskIndex;

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
        ) mutable -> TypedTaskIndex<void>   //
        {
            std::vector<TypedTaskIndex<void>> task_indices{
                std::from_range,
                std::move(task_builders) | std::views::as_rvalue
                    | std::views::transform(
                        [&task_hub_builder](TaskBuilder<void>&& builder)
                            -> TypedTaskIndex<void>   //
                        {                             //
                            return std::move(builder).build(task_hub_builder);
                        }
                    )
            };

            // TODO: use std::views::adjacent
            for (const std::size_t previous_task_index_index :
                 std::views::iota(0uz, task_indices.size() - 1))
            {
                task_hub_builder.set_task_continuation_factory(
                    task_indices[previous_task_index_index],
                    TaskContinuationFactory<void>{
                        [next_task_index = task_indices[previous_task_index_index + 1]](
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

            std::shared_ptr<std::optional<TaskContinuation<void>>> shared_continuation{
                std::make_shared<std::optional<TaskContinuation<void>>>()
            };

            task_hub_builder.set_task_continuation_factory(
                task_indices.back(),
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
            for (const TypedTaskIndex<void> task_index : task_indices) {
                locks.expand(task_hub_builder.locks_of(task_index));
            }

            return task_hub_builder.emplace_indirect_task_factory(
                IndirectTaskFactory<void>{
                    task_indices.front(),
                    IndirectTaskContinuationSetter<void>{
                        [shared_continuation](TaskContinuation<void>&& continuation)
                            -> void {
                            (*shared_continuation) = std::move(continuation);
                        }   //
                    },
                    std::move(locks)   //
                }
            );
        }   //
    };
}
