module;

#include <concepts>
#include <utility>

export module ddge.scheduler.primitives.all_of;

import ddge.scheduler.gatherers.AllOf;
import ddge.scheduler.TaskBuilder;
import ddge.scheduler.TaskBuilderGroup;

namespace ddge::scheduler {

export template <typename... TaskBuilderGroups_T>
    requires std::constructible_from<TaskBuilderGroup<bool>, TaskBuilderGroups_T&&...>
[[nodiscard]]
auto all_of(TaskBuilderGroups_T&&... builders) -> TaskBuilder<bool>
{
    return TaskBuilderGroup<bool>{ std::move(builders)... }.sync(AllOfBuilder{});
}

}   // namespace ddge::scheduler
