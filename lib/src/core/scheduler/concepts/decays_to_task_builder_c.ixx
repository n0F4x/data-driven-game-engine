module;

#include <type_traits>

export module core.scheduler.concepts.decays_to_task_builder_c;

import core.scheduler.concepts.task_builder_c;

namespace core::scheduler {

export template <typename T>
concept decays_to_task_builder_c = task_builder_c<std::remove_cvref_t<T>>;

}   // namespace core::scheduler
