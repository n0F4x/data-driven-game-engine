module;

#include <concepts>

export module ddge.modules.execution.v2.convertible_to_TaskBlueprint_c;

import ddge.modules.execution.v2.Cardinality;
import ddge.modules.execution.v2.TaskBlueprint;

namespace ddge::exec::v2 {

export template <typename T, typename Result_T>
concept convertible_to_TaskBlueprint_c = requires {
    static_cast<TaskBlueprint<Result_T, Cardinality::eSingle>>(std::declval<T&&>());
} || requires {
    static_cast<TaskBlueprint<Result_T, Cardinality::eMulti>>(std::declval<T&&>());
};

}   // namespace ddge::exec::v2
