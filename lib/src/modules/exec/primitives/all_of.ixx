module;

#include <utility>

export module ddge.modules.exec.primitives.all_of;

import ddge.modules.exec.as_task_blueprint;
import ddge.modules.exec.Cardinality;
import ddge.modules.exec.convertible_to_TaskBlueprint_c;
import ddge.modules.exec.gatherers.AllOf;
import ddge.modules.exec.TaskBlueprint;
import ddge.modules.exec.TaskBuilder;
import ddge.modules.exec.TaskBuilderBundle;

namespace ddge::exec {

export template <typename... TaskBlueprints_T>
    requires(sizeof...(TaskBlueprints_T) > 0)
         && (convertible_to_TaskBlueprint_c<TaskBlueprints_T, bool> && ...)
auto all_of(TaskBlueprints_T&&... blueprints)
    -> TaskBlueprint<bool, Cardinality::eSingle>;

}   // namespace ddge::exec

template <typename... TaskBlueprints_T>
    requires(sizeof...(TaskBlueprints_T) > 0)
         && (ddge::exec::convertible_to_TaskBlueprint_c<TaskBlueprints_T, bool> && ...)
auto ddge::exec::all_of(TaskBlueprints_T&&... blueprints)
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
