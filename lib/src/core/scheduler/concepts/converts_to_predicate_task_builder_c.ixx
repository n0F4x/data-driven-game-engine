export module core.scheduler.concepts.converts_to_predicate_task_builder_c;

import core.scheduler.as_task_builder;
import core.scheduler.concepts.predicate_task_builder_c;

namespace core::scheduler {

export template <typename T>
concept converts_to_predicate_task_builder_c = predicate_task_builder_c<as_task_builder_t<T>>;

}   // namespace core::scheduler
