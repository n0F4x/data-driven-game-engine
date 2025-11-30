module;

#include <concepts>

export module ddge.modules.exec.convertible_to_TaskBlueprint_c;

import ddge.modules.exec.Cardinality;
import ddge.modules.exec.TaskBlueprint;

namespace ddge::exec {

export template <typename T, typename Result_T>
concept convertible_to_TaskBlueprint_c = requires {
    static_cast<TaskBlueprint<Result_T, Cardinality::eSingle>>(std::declval<T&&>());
} || requires {
    static_cast<TaskBlueprint<Result_T, Cardinality::eMulti>>(std::declval<T&&>());
};

}   // namespace ddge::exec
