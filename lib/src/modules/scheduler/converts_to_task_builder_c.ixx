module;

#include <type_traits>

export module modules.scheduler.converts_to_task_builder_c;

import modules.scheduler.as_task_builder_t;

namespace modules::scheduler {

export template <typename T>
concept converts_to_task_builder_c = requires {
    std::type_identity<as_task_builder_t<std::remove_cvref_t<T>>>{};
};

}   // namespace modules::scheduler
