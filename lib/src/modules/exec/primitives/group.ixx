module;

#include <utility>

export module ddge.modules.exec.primitives.group;

import ddge.modules.exec.as_task_blueprint;
import ddge.modules.exec.Cardinality;
import ddge.modules.exec.convertible_to_TaskBlueprint_c;
import ddge.modules.exec.TaskBlueprint;
import ddge.modules.exec.TaskBuilderBundle;

namespace ddge::exec {

template <typename... TaskBlueprints_T>
inline constexpr ddge::exec::Cardinality cardinality_result{
    ((sizeof...(TaskBlueprints_T) == 1)
     && TaskBlueprints_T...[0] ::cardinality() == ddge::exec::Cardinality::eSingle)
        ? ddge::exec::Cardinality::eSingle
        : ddge::exec::Cardinality::eMulti   //
};

export template <typename... TaskBlueprints_T>
    requires(sizeof...(TaskBlueprints_T) > 0)
         && (convertible_to_TaskBlueprint_c<TaskBlueprints_T, void> && ...)
auto group(TaskBlueprints_T&&... blueprints)
    -> TaskBlueprint<void, cardinality_result<TaskBlueprints_T...>>;

}   // namespace ddge::exec

template <typename... TaskBlueprints_T>
    requires(sizeof...(TaskBlueprints_T) > 0)
         && (ddge::exec::convertible_to_TaskBlueprint_c<TaskBlueprints_T, void> && ...)
auto ddge::exec::group(TaskBlueprints_T&&... blueprints)
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
