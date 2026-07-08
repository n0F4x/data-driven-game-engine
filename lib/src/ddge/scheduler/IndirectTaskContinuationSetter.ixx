export module ddge.scheduler.IndirectTaskContinuationSetter;

import ddge.scheduler.TaskContinuation;

import ddge.util.containers.MoveOnlyFunction;

namespace ddge::scheduler {

export template <typename Result_T>
using IndirectTaskContinuationSetter =
    util::MoveOnlyFunction<void(TaskContinuation<Result_T>&&) &&>;

}   // namespace ddge::scheduler
