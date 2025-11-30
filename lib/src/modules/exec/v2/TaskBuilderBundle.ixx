module;

#include <algorithm>
#include <cstdint>
#include <memory>
#include <ranges>
#include <span>
#include <vector>

export module ddge.modules.exec.v2.TaskBuilderBundle;

import ddge.modules.exec.locks.DependencyStack;
import ddge.modules.exec.locks.LockGroup;
import ddge.modules.exec.locks.LockOwnerIndex;
import ddge.modules.exec.v2.gatherers.gatherer_builder_of_c;
import ddge.modules.exec.v2.gatherers.WaitAll;
import ddge.modules.exec.v2.IndirectTaskContinuationSetter;
import ddge.modules.exec.v2.IndirectTaskFactory;
import ddge.modules.exec.v2.TaskBuilder;
import ddge.modules.exec.v2.TaskContinuation;
import ddge.modules.exec.v2.TaskContinuationFactory;
import ddge.modules.exec.v2.TaskHubBuilder;
import ddge.modules.exec.v2.TaskHubProxy;
import ddge.modules.exec.v2.TaskIndex;
import ddge.modules.exec.v2.TypedTaskIndex;

namespace ddge::exec::v2 {

export template <typename Result_T>
class TaskBuilderBundle {
public:
    auto emplace(TaskBuilder<Result_T>&& task_builder) -> void;
    auto emplace(TaskBuilderBundle&& task_builder_bundle) -> void;

    template <gatherer_builder_of_c<Result_T> GathererBuilder_T>
    [[nodiscard]]
    auto sync(
        GathererBuilder_T&& gatherer_builder
    ) && -> TaskBuilder<typename GathererBuilder_T::Result>;

private:
    std::vector<TaskBuilder<Result_T>> m_task_builders;
};

}   // namespace ddge::exec::v2

template <typename Result_T>
auto ddge::exec::v2::TaskBuilderBundle<Result_T>::emplace(
    TaskBuilder<Result_T>&& task_builder
) -> void
{
    m_task_builders.emplace_back(std::move(task_builder));
}

template <typename Result_T>
auto ddge::exec::v2::TaskBuilderBundle<Result_T>::emplace(
    TaskBuilderBundle&& task_builder_bundle
) -> void
{
    m_task_builders.append_range(std::move(task_builder_bundle).m_task_builders);
}

template <typename Result_T>
template <ddge::exec::v2::gatherer_builder_of_c<Result_T> GathererBuilder_T>
auto ddge::exec::v2::TaskBuilderBundle<Result_T>::sync(
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
