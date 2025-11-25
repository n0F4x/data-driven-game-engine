export module ddge.modules.exec.v2.TaskContinuationFactory;

import ddge.modules.exec.v2.TaskContinuation;
import ddge.modules.exec.v2.TaskHubProxy;
import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::exec::v2 {

export template <typename Result_T>
using TaskContinuationFactory =
    util::AnyMoveOnlyFunction<auto(TaskHubProxy)&&->TaskContinuation<Result_T>>;

}   // namespace ddge::exec::v2
