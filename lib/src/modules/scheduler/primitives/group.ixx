module;

#include <concepts>
#include <utility>

export module ddge.modules.scheduler.primitives.group;

import ddge.modules.scheduler.TaskBuilderGroup;

namespace ddge::scheduler {

export template <typename... TaskBuilderGroups_T>
    requires std::constructible_from<TaskBuilderGroup<void>, TaskBuilderGroups_T&&...>
[[nodiscard]]
auto group(TaskBuilderGroups_T&&... builders) -> TaskBuilderGroup<void>
{
    return TaskBuilderGroup<void>{ std::forward<TaskBuilderGroups_T>(builders)... };
}

}   // namespace ddge::scheduler
