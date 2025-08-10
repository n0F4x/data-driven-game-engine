module;

#include <utility>

export module ddge.modules.execution.as_task_builder_t;

import ddge.modules.execution.wrap_as_builder;

import ddge.utility.containers.FunctionWrapper;

namespace ddge::exec {

export template <typename T>
using as_task_builder_t = decltype(wrap_as_builder(std::declval<T>()));

}   // namespace ddge::exec
