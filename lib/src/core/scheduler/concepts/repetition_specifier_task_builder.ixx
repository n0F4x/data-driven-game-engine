module;

#include <concepts>

export module core.scheduler.concepts.repetition_specifier_task_builder_c;

import core.scheduler.concepts.task_builder_c;

namespace core::scheduler {

export template <typename T>
concept repetition_specifier_task_builder_c = task_builder_c<T>
                                           && std::integral<typename T::Result>;

}   // namespace core::scheduler
