module;

#include <algorithm>
#include <cstdint>
#include <memory>
#include <ranges>
#include <vector>

export module ddge.modules.exec.v2.TaskBuilderBundle;

import ddge.modules.exec.v2.gatherers.gatherer_builder_of_c;
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

            for (const TypedTaskIndex<Result_T> task_index : task_indices) {
                task_hub_builder.set_task_continuation_factory(
                    task_index,
                    TaskContinuationFactory<Result_T>{
                        [gatherer](const TaskHubProxy) mutable
                            -> TaskContinuation<Result_T> {
                            return TaskContinuation<Result_T>{
                                [gatherer]<typename... XInputs_T>(
                                    XInputs_T&&... inputs
                                ) mutable -> void {
                                    gatherer->receive(std::forward<XInputs_T>(inputs)...);
                                }   //
                            };
                        }   //
                    }
                );
            }

            return task_hub_builder.emplace_indirect_task_factory(
                IndirectTaskFactory<NewResult>{
                    std::move(task_indices)
                        | std::views::transform(
                            [](const TypedTaskIndex<Result_T> typed_task_index)
                                -> TaskIndex { return typed_task_index.untyped(); }
                        )
                        | std::ranges::to<std::vector>(),
                    IndirectTaskContinuationSetter<NewResult>{
                        [gatherer](TaskContinuation<Result_T>&& continuation) mutable
                            -> void {
                            gatherer->set_continuation(std::move(continuation));
                        }   //
                    }   //
                }
            );
        }   //
    };
}
