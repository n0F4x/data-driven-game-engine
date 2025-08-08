module;

#include <utility>

export module modules.scheduler.as_task_builder_t;

import modules.scheduler.wrap_as_builder;

import utility.containers.FunctionWrapper;

namespace modules::scheduler {

export template <typename T>
using as_task_builder_t = decltype(wrap_as_builder(std::declval<T>()));

}   // namespace modules::scheduler
