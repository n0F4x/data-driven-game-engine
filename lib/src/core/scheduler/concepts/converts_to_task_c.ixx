module;

#include <type_traits>

export module core.scheduler.concepts.converts_to_task_c;

import core.scheduler.as_task;

namespace core::scheduler {

export template <typename T>
concept converts_to_task_c = requires { std::type_identity<as_task_t<T>>{}; };

}   // namespace core::scheduler
