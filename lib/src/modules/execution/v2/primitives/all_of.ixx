module;

#include <utility>

export module ddge.modules.execution.v2.primitives.all_of;

import ddge.modules.execution.v2.as_task_blueprint;
import ddge.modules.execution.v2.Cardinality;
import ddge.modules.execution.v2.convertible_to_TaskBlueprint_c;
import ddge.modules.execution.v2.gatherers.AllOf;
import ddge.modules.execution.v2.TaskBlueprint;
import ddge.modules.execution.v2.TaskBuilder;
import ddge.modules.execution.v2.TaskBuilderBundle;

namespace ddge::exec::v2 {

export template <typename... TaskBlueprints_T>
    requires(sizeof...(TaskBlueprints_T) > 0)
         && (convertible_to_TaskBlueprint_c<TaskBlueprints_T, bool> && ...)
auto all_of(TaskBlueprints_T&&... blueprints)
    -> TaskBlueprint<bool, Cardinality::eSingle>;

}   // namespace ddge::exec::v2

template <typename... TaskBlueprints_T>
    requires(sizeof...(TaskBlueprints_T) > 0)
         && (ddge::exec::v2::convertible_to_TaskBlueprint_c<TaskBlueprints_T, bool> && ...)
auto ddge::exec::v2::all_of(TaskBlueprints_T&&... blueprints)
    -> TaskBlueprint<bool, Cardinality::eSingle>
{
    return TaskBlueprint<bool, Cardinality::eSingle>{
        [... x_blueprints = as_task_blueprint<bool>(std::move(blueprints)
         )] mutable -> TaskBuilder<bool> {
            TaskBuilderBundle<bool> bundle;

            (bundle.emplace(std::move(x_blueprints).materialize()), ...);

            return std::move(bundle).sync(AllOfBuilder{});
        }   //
    };
}
