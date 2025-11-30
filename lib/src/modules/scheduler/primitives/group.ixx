module;

#include <utility>

export module ddge.modules.scheduler.primitives.group;

import ddge.modules.scheduler.as_task_blueprint;
import ddge.modules.scheduler.Cardinality;
import ddge.modules.scheduler.convertible_to_TaskBlueprint_c;
import ddge.modules.scheduler.TaskBlueprint;
import ddge.modules.scheduler.TaskBuilderBundle;

namespace ddge::scheduler {

template <typename... TaskBlueprints_T>
inline constexpr ddge::scheduler::Cardinality cardinality_result{
    ((sizeof...(TaskBlueprints_T) == 1)
     && TaskBlueprints_T...[0] ::cardinality() == ddge::scheduler::Cardinality::eSingle)
        ? ddge::scheduler::Cardinality::eSingle
        : ddge::scheduler::Cardinality::eMulti   //
};

export template <typename... TaskBlueprints_T>
    requires(sizeof...(TaskBlueprints_T) > 0)
         && (convertible_to_TaskBlueprint_c<TaskBlueprints_T, void> && ...)
auto group(TaskBlueprints_T&&... blueprints)
    -> TaskBlueprint<void, cardinality_result<TaskBlueprints_T...>>;

}   // namespace ddge::scheduler

template <typename... TaskBlueprints_T>
    requires(sizeof...(TaskBlueprints_T) > 0)
         && (ddge::scheduler::convertible_to_TaskBlueprint_c<TaskBlueprints_T, void>
             && ...)
auto ddge::scheduler::group(TaskBlueprints_T&&... blueprints)
    -> TaskBlueprint<void, cardinality_result<TaskBlueprints_T...>>
{
    return TaskBlueprint<void, cardinality_result<TaskBlueprints_T...>>{
        [... x_blueprints = as_task_blueprint<void>(std::move(blueprints))] mutable
            -> TaskBuilderBundle<void> {
            TaskBuilderBundle<void> bundle;

            (bundle.emplace(std::move(x_blueprints).materialize()), ...);

            return bundle;
        }   //
    };
}
