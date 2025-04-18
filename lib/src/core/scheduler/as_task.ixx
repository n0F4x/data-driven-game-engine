module;

#include <type_traits>

export module core.scheduler.as_task;

import utility.meta.concepts.functional.function;
import utility.meta.concepts.functional.member_function_pointer;
import utility.meta.concepts.functional.unambiguous_functor;

import core.scheduler.concepts.task_wrapper_c;
import core.scheduler.Task;

namespace core::scheduler {

template <typename T>
struct AsTask;

template <typename T>
    requires util::meta::function_c<std::remove_reference_t<T>>
struct AsTask<T> {
    using type = Task<std::remove_reference_t<T>>;
};

template <typename T>
    requires util::meta::member_function_pointer_c<std::decay_t<T>>
struct AsTask<T> {
    using type = Task<std::decay_t<T>>;
};

template <typename T>
    requires util::meta::unambiguous_functor_c<std::remove_cvref_t<T>>
struct AsTask<T> {
    using type = Task<std::remove_cvref_t<T>>;
};

template <typename T>
    requires task_wrapper_c<std::remove_cvref_t<T>>
struct AsTask<T> {
    using type = std::remove_cvref_t<T>;
};

export template <typename T>
using as_task_t = typename AsTask<T>::type;

}   // namespace core::scheduler
