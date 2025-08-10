module;

#include <utility>

export module ddge.modules.scheduler.as_task_builder_t;

import ddge.modules.scheduler.wrap_as_builder;

import ddge.utility.containers.FunctionWrapper;

namespace ddge::scheduler {

export template <typename T>
using as_task_builder_t = decltype(wrap_as_builder(std::declval<T>()));

}   // namespace ddge::scheduler
