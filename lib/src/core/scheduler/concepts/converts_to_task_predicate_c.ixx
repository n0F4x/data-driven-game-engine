export module core.scheduler.concepts.converts_to_task_predicate_c;

import core.scheduler.as_task;
import core.scheduler.concepts.task_predicate_c;

namespace core::scheduler {

export template <typename T>
concept converts_to_task_predicate_c = task_predicate_c<as_task_t<T>>;

}   // namespace core::scheduler
