module;

#include <type_traits>

export module core.scheduler.concepts.task_c;

import utility.meta.concepts.functional.function;
import utility.meta.concepts.functional.member_function_pointer;
import utility.meta.concepts.functional.unambiguous_functor;

import core.scheduler.concepts.task_wrapper_c;
import core.scheduler.Task;

namespace core::scheduler {

export template <typename T>
concept task_c = requires { std::type_identity<Task<T>>{}; } || task_wrapper_c<T>;

}   // namespace core::scheduler
