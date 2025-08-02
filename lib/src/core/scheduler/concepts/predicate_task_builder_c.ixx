module;

#include <concepts>

export module core.scheduler.concepts.predicate_task_builder_c;

import core.scheduler.concepts.task_builder_c;

namespace core::scheduler {

export template <typename T>
concept predicate_task_builder_c = task_builder_c<T>
                                && std::convertible_to<typename T::Result, bool>;

}   // namespace core::scheduler
