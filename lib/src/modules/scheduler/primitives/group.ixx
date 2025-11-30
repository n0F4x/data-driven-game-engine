module;

#include <concepts>
#include <utility>

export module ddge.modules.scheduler.primitives.group;

import ddge.modules.scheduler.TaskBuilderBundle;

namespace ddge::scheduler {

export template <typename... TaskBuildersOrBundles_T>
    requires std::constructible_from<TaskBuilderBundle<void>, TaskBuildersOrBundles_T&&...>
[[nodiscard]]
auto group(TaskBuildersOrBundles_T&&... builders) -> TaskBuilderBundle<void>
{
    return TaskBuilderBundle<void>{ std::forward<TaskBuildersOrBundles_T>(builders)... };
}

}   // namespace ddge::scheduler
