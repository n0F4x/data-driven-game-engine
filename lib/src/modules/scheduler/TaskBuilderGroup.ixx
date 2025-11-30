module;

#include <algorithm>
#include <cstdint>
#include <memory>
#include <ranges>
#include <span>
#include <vector>

export module ddge.modules.scheduler.TaskBuilderGroup;

import ddge.modules.scheduler.locks.DependencyStack;
import ddge.modules.scheduler.locks.LockGroup;
import ddge.modules.scheduler.locks.LockOwnerIndex;
import ddge.modules.scheduler.gatherers.gatherer_builder_of_c;
import ddge.modules.scheduler.gatherers.WaitAll;
import ddge.modules.scheduler.IndirectTaskContinuationSetter;
import ddge.modules.scheduler.IndirectTaskFactory;
import ddge.modules.scheduler.TaskBuilder;
import ddge.modules.scheduler.TaskContinuation;
import ddge.modules.scheduler.TaskContinuationFactory;
import ddge.modules.scheduler.TaskHubBuilder;
import ddge.modules.scheduler.TaskHubProxy;
import ddge.modules.scheduler.TaskIndex;
import ddge.modules.scheduler.TaskResultTrait;
import ddge.modules.scheduler.TypedTaskIndex;

import ddge.utility.meta.concepts.strips_to;

namespace ddge::scheduler {

export template <typename Result_T>
class TaskBuilderGroup {
public:
    TaskBuilderGroup() = default;

    template <typename... TaskBuilderGroups_T>
    explicit TaskBuilderGroup(TaskBuilderGroups_T&&... task_builder_groups)
        requires((util::meta::strips_to_c<TaskBuilderGroups_T, TaskBuilder<Result_T>>
                  || util::meta::strips_to_c<TaskBuilderGroups_T, TaskBuilderGroup>)
                 && ...)
             && (!(
                 sizeof...(TaskBuilderGroups_T) == 1
                 && util::meta::strips_to_c<TaskBuilderGroups_T...[0], TaskBuilderGroup>
             ));

    auto emplace(TaskBuilder<Result_T>&& task_builder) -> void;
    auto emplace(TaskBuilderGroup&& task_builder_group) -> void;

    template <gatherer_builder_of_c<Result_T> GathererBuilder_T>
    [[nodiscard]]
    auto sync(
        GathererBuilder_T&& gatherer_builder
    ) && -> TaskBuilder<typename GathererBuilder_T::Result>;

    [[nodiscard]]
    explicit(false) operator TaskBuilder<void>() &&
        requires std::same_as<Result_T, void>;

    [[nodiscard]]
    auto size() const noexcept -> uint32_t;

private:
    std::vector<TaskBuilder<Result_T>> m_task_builders;
};

export template <typename Result_T>
struct TaskResultTrait<TaskBuilderGroup<Result_T>> {
    using type = Result_T;
};

}   // namespace ddge::scheduler

template <typename Result_T>
[[nodiscard]]
auto count_task_builders(const ddge::scheduler::TaskBuilder<Result_T>&) noexcept
    -> uint32_t
{
    return 1;
}

template <typename Result_T>
[[nodiscard]]
auto count_task_builders(
    const ddge::scheduler::TaskBuilderGroup<Result_T>& task_builder_group
) noexcept -> uint32_t
{
    return task_builder_group.size();
}

template <typename Result_T>
template <typename... TaskBuilderGroups_T>
ddge::scheduler::TaskBuilderGroup<Result_T>::TaskBuilderGroup(
    TaskBuilderGroups_T&&... task_builder_groups
)
    requires((util::meta::strips_to_c<TaskBuilderGroups_T, TaskBuilder<Result_T>>
              || util::meta::strips_to_c<TaskBuilderGroups_T, TaskBuilderGroup>)
             && ...)
         && (!(
             sizeof...(TaskBuilderGroups_T) == 1
             && util::meta::strips_to_c<TaskBuilderGroups_T...[0], TaskBuilderGroup>
         ))
{
    m_task_builders.reserve((::count_task_builders(task_builder_groups) + ...));
    (emplace(std::move(task_builder_groups)), ...);
}

template <typename Result_T>
auto ddge::scheduler::TaskBuilderGroup<Result_T>::emplace(
    TaskBuilder<Result_T>&& task_builder
) -> void
{
    m_task_builders.emplace_back(std::move(task_builder));
}

template <typename Result_T>
auto ddge::scheduler::TaskBuilderGroup<Result_T>::emplace(
    TaskBuilderGroup&& task_builder_group
) -> void
{
    m_task_builders.append_range(std::move(task_builder_group).m_task_builders);
}

template <typename Result_T>
template <ddge::scheduler::gatherer_builder_of_c<Result_T> GathererBuilder_T>
auto ddge::scheduler::TaskBuilderGroup<Result_T>::sync(
    GathererBuilder_T&& gatherer_builder
) && -> TaskBuilder<typename GathererBuilder_T::Result>
{
    using NewResult = typename GathererBuilder_T::Result;

    return TaskBuilder<NewResult>{
        [builders           = std::move(m_task_builders),
         x_gatherer_builder = std::move(gatherer_builder)]   //
        (                                                    //
            TaskHubBuilder & task_hub_builder
        ) mutable -> TypedTaskIndex<NewResult>               //
        {
            const std::shared_ptr gatherer{
                std::move(x_gatherer_builder).build(static_cast<uint32_t>(builders.size()))
            };

            std::vector<TypedTaskIndex<Result_T>> task_indices{
                std::from_range,
                std::move(builders) | std::views::as_rvalue
                    | std::views::transform(
                        [&task_hub_builder]   //
                        (TaskBuilder<Result_T>&& builder) -> TypedTaskIndex<Result_T> {
                            return std::move(builder).build(task_hub_builder);
                        }
                    )
            };

            DependencyStack                       dependency_stack;
            std::vector<std::shared_ptr<WaitAll>> intermediate_waits(task_indices.size());
            for (const TypedTaskIndex<Result_T> task_index : task_indices) {
                const LockOwnerIndex lock_owner_index =
                    dependency_stack.emplace(task_hub_builder.locks_of(task_index));

                if (const std::span<const LockOwnerIndex> dependencies{
                        dependency_stack.dependencies_of(lock_owner_index) };
                    dependencies.size() > 1)
                {
                    intermediate_waits[lock_owner_index] =
                        std::make_shared<WaitAll>(dependencies.size());
                }
            }

            for (const LockOwnerIndex lock_owner_index :
                 std::views::iota(0u, dependency_stack.size()))
            {
                std::vector<TypedTaskIndex<Result_T>> dependent_task_indices{
                    std::from_range,
                    dependency_stack.dependees_of(lock_owner_index)
                        | std::views::transform([&task_indices](
                                                    const LockOwnerIndex dependee_index
                                                ) { return task_indices[dependee_index]; })
                };

                std::vector<std::shared_ptr<WaitAll>> dependent_intermediate_waits{
                    std::from_range,
                    dependency_stack.dependees_of(lock_owner_index)
                        | std::views::transform(
                            [&intermediate_waits](const LockOwnerIndex dependee_index) {
                                return intermediate_waits[dependee_index];
                            }
                        )
                };

                auto schedule_dependent_continuations_factory{
                    [x_dependent_task_indices       = std::move(dependent_task_indices),
                     x_dependent_intermediate_waits = std::move(
                         dependent_intermediate_waits
                     )](const TaskHubProxy task_hub_proxy) mutable {
                        return [y_dependent_task_indices =
                                    std::move(x_dependent_task_indices),
                                y_dependent_intermediate_waits =
                                    std::move(x_dependent_intermediate_waits),
                                task_hub_proxy] mutable -> void {
                            for (const auto& [dependent_task_index, dependent_intermediate_wait] :
                                 std::views::zip(
                                     y_dependent_task_indices,
                                     y_dependent_intermediate_waits
                                 ))
                            {
                                if (dependent_intermediate_wait == nullptr) {
                                    task_hub_proxy.schedule(dependent_task_index);
                                }
                                else {
                                    dependent_intermediate_wait->receive();
                                }
                            }
                        };
                    }
                };

                task_hub_builder.set_task_continuation_factory(
                    task_indices[lock_owner_index],
                    TaskContinuationFactory<Result_T>{
                        [gatherer,
                         x_schedule_dependent_continuations_factory =
                             std::move(schedule_dependent_continuations_factory)](
                            const TaskHubProxy task_hub_proxy
                        ) mutable -> TaskContinuation<Result_T> {
                            return TaskContinuation<Result_T>{
                                [gatherer,
                                 schedule_dependent_continuations =
                                     std::move(x_schedule_dependent_continuations_factory)(
                                         task_hub_proxy
                                     )]<typename... XInputs_T>(
                                    XInputs_T&&... inputs
                                ) mutable -> void {
                                    gatherer->receive(std::forward<XInputs_T>(inputs)...);
                                    schedule_dependent_continuations();
                                }   //
                            };
                        }   //
                    }
                );
            }

            return task_hub_builder.emplace_indirect_task_factory(
                IndirectTaskFactory<NewResult>{
                    dependency_stack.bottom()
                        | std::views::transform(
                            [&task_indices](const LockOwnerIndex lock_owner_index)
                                -> TaskIndex {
                                return task_indices[lock_owner_index].untyped();
                            }
                        )
                        | std::ranges::to<std::vector>(),
                    IndirectTaskContinuationSetter<NewResult>{
                        [gatherer](TaskContinuation<Result_T>&& continuation) mutable
                            -> void {
                            gatherer->set_continuation(std::move(continuation));
                        }   //
                    },
                    LockGroup{ dependency_stack.locks() }   //
                }
            );
        }   //
    };
}

template <typename Result_T>
ddge::scheduler::TaskBuilderGroup<Result_T>::operator TaskBuilder<void>() &&
    requires std::same_as<Result_T, void>
{
    return std::move(*this).sync(WaitAllBuilder{});
}

template <typename Result_T>
auto ddge::scheduler::TaskBuilderGroup<Result_T>::size() const noexcept -> uint32_t
{
    return m_task_builders.size();
}
