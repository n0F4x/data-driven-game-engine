export module ddge.modules.exec.v2.IndirectTaskContinuationSetter;

import ddge.modules.exec.v2.TaskContinuation;

import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::exec::v2 {

export template <typename Result_T>
using IndirectTaskContinuationSetter =
    util::AnyMoveOnlyFunction<void(TaskContinuation<Result_T>&&) &&>;

}   // namespace ddge::exec::v2
