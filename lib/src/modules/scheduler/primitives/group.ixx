module;

#include <concepts>
#include <utility>

export module ddge.modules.scheduler.primitives.group;

import ddge.modules.scheduler.TaskBuilderGroup;
import ddge.modules.scheduler.TaskResultTrait;

import ddge.utility.meta.concepts.all_same;

namespace ddge::scheduler {

export template <typename... TaskBuilderGroups_T>
    requires requires {
        util::meta::
            all_same_c<task_result_of_t<std::remove_cvref_t<TaskBuilderGroups_T>>...>;
    }
          && std::constructible_from<
                 TaskBuilderGroup<
                     task_result_of_t<std::remove_cvref_t<TaskBuilderGroups_T...[0]>>>,
                 TaskBuilderGroups_T&&...>
[[nodiscard]]
auto group(TaskBuilderGroups_T&&... builders)
    -> TaskBuilderGroup<task_result_of_t<std::remove_cvref_t<TaskBuilderGroups_T...[0]>>>
{
    return TaskBuilderGroup<
        task_result_of_t<std::remove_cvref_t<TaskBuilderGroups_T...[0]>>>{
        std::forward<TaskBuilderGroups_T>(builders)...
    };
}

}   // namespace ddge::scheduler
