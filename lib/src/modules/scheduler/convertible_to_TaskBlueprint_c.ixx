module;

#include <concepts>

export module ddge.modules.scheduler.convertible_to_TaskBlueprint_c;

import ddge.modules.scheduler.Cardinality;
import ddge.modules.scheduler.TaskBlueprint;

namespace ddge::scheduler {

export template <typename T, typename Result_T>
concept convertible_to_TaskBlueprint_c = requires {
    static_cast<TaskBlueprint<Result_T, Cardinality::eSingle>>(std::declval<T&&>());
} || requires {
    static_cast<TaskBlueprint<Result_T, Cardinality::eMulti>>(std::declval<T&&>());
};

}   // namespace ddge::scheduler
