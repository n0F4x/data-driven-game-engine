module;

#include <concepts>

export module core.scheduler.concepts.task_wrapper_c;

import utility.meta.concepts.type_list.type_list;

import core.scheduler.task_wrappers.TaskWrapperBase;

namespace core::scheduler {

export template <typename T>
concept task_wrapper_c = std::derived_from<T, TaskWrapperBase>
                      && util::meta::type_list_c<typename T::Dependencies>;

}   // namespace core::scheduler
