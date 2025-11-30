export module ddge.modules.scheduler.IndirectTaskContinuationSetter;

import ddge.modules.scheduler.TaskContinuation;

import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::scheduler {

export template <typename Result_T>
using IndirectTaskContinuationSetter =
    util::AnyMoveOnlyFunction<void(TaskContinuation<Result_T>&&) &&>;

}   // namespace ddge::scheduler
