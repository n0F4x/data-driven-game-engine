module;

#include <concepts>
#include <utility>

export module ddge.modules.scheduler.primitives.all_of;

import ddge.modules.scheduler.gatherers.AllOf;
import ddge.modules.scheduler.TaskBuilder;
import ddge.modules.scheduler.TaskBuilderBundle;

namespace ddge::scheduler {

export template <typename... TaskBuildersOrBundles_T>
    requires std::constructible_from<TaskBuilderBundle<bool>, TaskBuildersOrBundles_T&&...>
[[nodiscard]]
auto all_of(TaskBuildersOrBundles_T&&... builders) -> TaskBuilder<bool>
{
    return TaskBuilderBundle<bool>{ std::move(builders)... }.sync(AllOfBuilder{});
}

}   // namespace ddge::scheduler
