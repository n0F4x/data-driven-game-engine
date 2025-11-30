export module ddge.modules.scheduler.TaskContinuationFactory;

import ddge.modules.scheduler.TaskContinuation;
import ddge.modules.scheduler.TaskHubProxy;
import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::scheduler {

export template <typename Result_T>
using TaskContinuationFactory =
    util::AnyMoveOnlyFunction<auto(TaskHubProxy) &&->TaskContinuation<Result_T>>;

}   // namespace ddge::scheduler
