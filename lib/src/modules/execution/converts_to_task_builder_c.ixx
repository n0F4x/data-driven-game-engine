module;

#include <type_traits>

export module ddge.modules.execution.converts_to_task_builder_c;

import ddge.modules.execution.as_task_builder_t;

namespace ddge::exec {

export template <typename T>
concept converts_to_task_builder_c = requires {
    std::type_identity<as_task_builder_t<std::remove_cvref_t<T>>>{};
};

}   // namespace ddge::exec
