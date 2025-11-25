module;

#include <algorithm>
#include <cstdint>
#include <memory>
#include <ranges>
#include <vector>

export module ddge.modules.exec.v2.TaskBuilderBundle;

import ddge.modules.exec.v2.gatherers.gatherer_builder_of_c;
import ddge.modules.exec.v2.TaskBuilder;
import ddge.modules.exec.v2.TaskBundle;
import ddge.modules.exec.v2.TaskFinishedCallback;
import ddge.modules.exec.v2.TaskHubBuilder;
import ddge.modules.exec.v2.TaskHubProxy;

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
            TaskHubBuilder & task_hub_builder,
            TaskFinishedCallback<NewResult>&& callback
        ) mutable -> TaskBundle   //
        {
            const std::shared_ptr gatherer{
                std::move(x_gatherer_builder)
                    .build(static_cast<uint32_t>(builders.size()), std::move(callback))
            };

            auto tasks =
                std::move(builders) | std::views::as_rvalue
                | std::views::transform(
                    [&task_hub_builder,
                     &gatherer]   //
                    (TaskBuilder<Result_T>&& builder) -> TaskBundle {
                        return std::move(builder).build(
                            task_hub_builder,
                            TaskFinishedCallback<Result_T>{
                                [gatherer]<typename... XInputs_T>(
                                    const TaskHubProxy&, XInputs_T&&... inputs
                                ) mutable -> void {
                                    gatherer->receive(std::forward<XInputs_T>(inputs)...);
                                }   //
                            }
                        );
                    }
                )
                | std::ranges::to<std::vector>();

            return TaskBundle{
                [gatherer, x_tasks = std::move(tasks)](
                    const TaskHubProxy& task_hub_proxy
                ) mutable -> void   //
                {
                    gatherer->set_task_hub_proxy(task_hub_proxy);

                    for (auto& task : x_tasks) {
                        task(task_hub_proxy);
                    }
                }   //
            };
        }   //
    };
}
