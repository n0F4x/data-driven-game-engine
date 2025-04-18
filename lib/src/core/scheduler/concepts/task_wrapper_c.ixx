module;

#include <concepts>

export module core.scheduler.concepts.task_wrapper_c;

import core.scheduler.task_wrappers.TaskWrapperBase;

namespace core::scheduler {

export template <typename T>
concept task_wrapper_c = std::derived_from<T, TaskWrapperBase>;

}   // namespace core::scheduler
