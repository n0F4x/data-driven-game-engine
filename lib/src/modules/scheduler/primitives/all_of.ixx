module;

#include <utility>

export module ddge.modules.scheduler.primitives.all_of;

import ddge.modules.scheduler.as_task_blueprint;
import ddge.modules.scheduler.Cardinality;
import ddge.modules.scheduler.convertible_to_TaskBlueprint_c;
import ddge.modules.scheduler.gatherers.AllOf;
import ddge.modules.scheduler.TaskBlueprint;
import ddge.modules.scheduler.TaskBuilder;
import ddge.modules.scheduler.TaskBuilderBundle;

namespace ddge::scheduler {

export template <typename... TaskBlueprints_T>
    requires(sizeof...(TaskBlueprints_T) > 0)
         && (convertible_to_TaskBlueprint_c<TaskBlueprints_T, bool> && ...)
auto all_of(TaskBlueprints_T&&... blueprints)
    -> TaskBlueprint<bool, Cardinality::eSingle>;

}   // namespace ddge::scheduler

template <typename... TaskBlueprints_T>
    requires(sizeof...(TaskBlueprints_T) > 0)
         && (ddge::scheduler::convertible_to_TaskBlueprint_c<TaskBlueprints_T, bool>
             && ...)
auto ddge::scheduler::all_of(TaskBlueprints_T&&... blueprints)
    -> TaskBlueprint<bool, Cardinality::eSingle>
{
    return TaskBlueprint<bool, Cardinality::eSingle>{
        [... x_blueprints = as_task_blueprint<bool>(std::move(blueprints))] mutable
            -> TaskBuilder<bool> {
            TaskBuilderBundle<bool> bundle;

            (bundle.emplace(std::move(x_blueprints).materialize()), ...);

            return std::move(bundle).sync(AllOfBuilder{});
        }   //
    };
}
