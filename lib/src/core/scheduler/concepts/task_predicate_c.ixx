module;

#include <concepts>

export module core.scheduler.concepts.task_predicate_c;

import core.scheduler.concepts.task_c;

namespace core::scheduler {

export template <typename T>
concept task_predicate_c = task_c<T> && std::convertible_to<typename T::Result, bool>;

}   // namespace core::scheduler
