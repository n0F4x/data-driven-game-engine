module;

#include <type_traits>

export module core.scheduler.concepts.decays_to_task_c;

import core.scheduler.concepts.task_c;

namespace core::scheduler {

export template <typename T>
concept decays_to_task_c = task_c<std::remove_cvref_t<T>>;

}   // namespace core::scheduler
