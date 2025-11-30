export module ddge.modules.scheduler.TaskResultTrait;

import ddge.utility.meta.concepts.naked;

namespace ddge::scheduler {

export template <util::meta::naked_c T>
struct TaskResultTrait;

export template <util::meta::naked_c T>
using task_result_of_t = typename TaskResultTrait<T>::type;

}   // namespace ddge::scheduler
