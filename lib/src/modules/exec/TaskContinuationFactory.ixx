export module ddge.modules.exec.TaskContinuationFactory;

import ddge.modules.exec.TaskContinuation;
import ddge.modules.exec.TaskHubProxy;
import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::exec {

export template <typename Result_T>
using TaskContinuationFactory =
    util::AnyMoveOnlyFunction<auto(TaskHubProxy) &&->TaskContinuation<Result_T>>;

}   // namespace ddge::exec
