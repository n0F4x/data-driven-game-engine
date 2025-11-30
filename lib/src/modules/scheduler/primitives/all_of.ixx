module;

#include <concepts>
#include <utility>

export module ddge.modules.scheduler.primitives.all_of;

import ddge.modules.scheduler.gatherers.AllOf;
import ddge.modules.scheduler.TaskBuilder;
import ddge.modules.scheduler.TaskBuilderGroup;

namespace ddge::scheduler {

export template <typename... TaskBuilderGroups_T>
    requires std::constructible_from<TaskBuilderGroup<bool>, TaskBuilderGroups_T&&...>
[[nodiscard]]
auto all_of(TaskBuilderGroups_T&&... builders) -> TaskBuilder<bool>
{
    return TaskBuilderGroup<bool>{ std::move(builders)... }.sync(AllOfBuilder{});
}

}   // namespace ddge::scheduler
