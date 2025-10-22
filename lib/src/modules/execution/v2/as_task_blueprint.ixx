module;

#include <concepts>
#include <utility>

export module ddge.modules.execution.v2.as_task_blueprint;

import ddge.modules.execution.v2.Cardinality;
import ddge.modules.execution.v2.TaskBlueprint;

namespace ddge::exec::v2 {

export template <typename Result_T, typename T>
[[nodiscard]]
auto as_task_blueprint(T&& task_blueprint)
{
    if constexpr (requires {
                      static_cast<TaskBlueprint<Result_T, Cardinality::eMulti>>(
                          std::move(task_blueprint)
                      );
                  })
    {
        return static_cast<TaskBlueprint<Result_T, Cardinality::eMulti>>(
            std::move(task_blueprint)
        );
    }
    else if constexpr (requires {
                           static_cast<TaskBlueprint<Result_T, Cardinality::eSingle>>(
                               std::move(task_blueprint)
                           );
                       })
    {
        return static_cast<TaskBlueprint<Result_T, Cardinality::eSingle>>(
            std::move(task_blueprint)
        );
    }
    else {
        static_assert(false);
    }
}

}   // namespace ddge::exec::v2
