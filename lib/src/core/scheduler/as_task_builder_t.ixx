module;

#include <utility>

export module core.scheduler.as_task_builder_t;

import core.scheduler.wrap_as_builder;

import utility.containers.FunctionWrapper;

namespace core::scheduler {

export template <typename T>
using as_task_builder_t = decltype(wrap_as_builder(std::declval<T>()));

}   // namespace core::scheduler
