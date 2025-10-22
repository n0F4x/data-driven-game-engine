module;

#include <utility>

export module ddge.modules.execution.v2.primitives.group;

import ddge.modules.execution.v2.as_task_blueprint;
import ddge.modules.execution.v2.Cardinality;
import ddge.modules.execution.v2.convertible_to_TaskBlueprint_c;
import ddge.modules.execution.v2.TaskBlueprint;
import ddge.modules.execution.v2.TaskBuilderBundle;

namespace ddge::exec::v2 {

template <typename... TaskBlueprints_T>
inline constexpr ddge::exec::v2::Cardinality cardinality_result{
    ((sizeof...(TaskBlueprints_T) == 1)
     && TaskBlueprints_T...[0] ::cardinality() == ddge::exec::v2::Cardinality::eSingle)
        ? ddge::exec::v2::Cardinality::eSingle
        : ddge::exec::v2::Cardinality::eMulti   //
};

export template <typename... TaskBlueprints_T>
    requires(sizeof...(TaskBlueprints_T) > 0)
         && (convertible_to_TaskBlueprint_c<TaskBlueprints_T, void> && ...)
auto group(TaskBlueprints_T&&... blueprints)
    -> TaskBlueprint<void, cardinality_result<TaskBlueprints_T...>>;

}   // namespace ddge::exec::v2

template <typename... TaskBlueprints_T>
    requires(sizeof...(TaskBlueprints_T) > 0)
         && (ddge::exec::v2::convertible_to_TaskBlueprint_c<TaskBlueprints_T, void> && ...)
auto ddge::exec::v2::group(TaskBlueprints_T&&... blueprints)
    -> TaskBlueprint<void, cardinality_result<TaskBlueprints_T...>>
{
    return TaskBlueprint<void, cardinality_result<TaskBlueprints_T...>>{
        [... x_blueprints = as_task_blueprint<void>(std::move(blueprints)
         )] mutable -> TaskBuilderBundle<void> {
            TaskBuilderBundle<void> bundle;

            (bundle.emplace(std::move(x_blueprints).materialize()), ...);

            return bundle;
        }   //
    };
}
