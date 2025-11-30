export module ddge.modules.exec.IndirectTaskContinuationSetter;

import ddge.modules.exec.TaskContinuation;

import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::exec {

export template <typename Result_T>
using IndirectTaskContinuationSetter =
    util::AnyMoveOnlyFunction<void(TaskContinuation<Result_T>&&) &&>;

}   // namespace ddge::exec
