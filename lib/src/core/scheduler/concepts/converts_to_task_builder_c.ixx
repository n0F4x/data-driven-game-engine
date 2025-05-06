module;

#include <type_traits>

export module core.scheduler.concepts.converts_to_task_builder_c;

import core.scheduler.as_task_builder;

namespace core::scheduler {

export template <typename T>
concept converts_to_task_builder_c = requires { std::type_identity<as_task_builder_t<T>>{}; };

}   // namespace core::scheduler
