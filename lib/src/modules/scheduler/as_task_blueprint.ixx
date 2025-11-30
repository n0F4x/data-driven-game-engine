module;

#include <concepts>
#include <utility>

export module ddge.modules.scheduler.as_task_blueprint;

import ddge.modules.scheduler.Cardinality;
import ddge.modules.scheduler.TaskBlueprint;

namespace ddge::scheduler {

export template <typename Result_T, Cardinality cardinality_T>
[[nodiscard]]
auto as_task_blueprint(TaskBlueprint<Result_T, cardinality_T>&& task_blueprint)
    -> TaskBlueprint<Result_T, cardinality_T>
{
    return std::move(task_blueprint);
}

export template <typename Result_T, typename T>
[[nodiscard]]
auto as_task_blueprint(T&& task_blueprint)
{
    if constexpr (requires {
                      std::move(task_blueprint)
                          .
                          operator TaskBlueprint<Result_T, Cardinality::eMulti>();
                  })
    {
        return std::move(task_blueprint)
            .operator TaskBlueprint<Result_T, Cardinality::eMulti>();
    }
    else if constexpr (requires {
                           std::move(task_blueprint)
                               .
                               operator TaskBlueprint<Result_T, Cardinality::eSingle>();
                       })
    {
        return std::move(task_blueprint)
            .operator TaskBlueprint<Result_T, Cardinality::eSingle>();
    }
    else {
        static_assert(false);
    }
}

}   // namespace ddge::scheduler
