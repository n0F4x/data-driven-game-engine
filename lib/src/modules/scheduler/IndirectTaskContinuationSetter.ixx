export module ddge.modules.scheduler.IndirectTaskContinuationSetter;

import ddge.modules.scheduler.TaskContinuation;

import ddge.utility.containers.MoveOnlyFunction;

namespace ddge::scheduler {

export template <typename Result_T>
using IndirectTaskContinuationSetter =
    util::MoveOnlyFunction<void(TaskContinuation<Result_T>&&) &&>;

}   // namespace ddge::scheduler
