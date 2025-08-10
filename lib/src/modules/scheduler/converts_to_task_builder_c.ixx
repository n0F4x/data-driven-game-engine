module;

#include <type_traits>

export module ddge.modules.scheduler.converts_to_task_builder_c;

import ddge.modules.scheduler.as_task_builder_t;

namespace ddge::scheduler {

export template <typename T>
concept converts_to_task_builder_c = requires {
    std::type_identity<as_task_builder_t<std::remove_cvref_t<T>>>{};
};

}   // namespace ddge::scheduler
