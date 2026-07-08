export module ddge.scheduler.TaskContinuationFactory;

import ddge.scheduler.TaskContinuation;
import ddge.scheduler.TaskHubProxy;
import ddge.util.containers.MoveOnlyFunction;

namespace ddge::scheduler {

export template <typename Result_T>
using TaskContinuationFactory =
    util::MoveOnlyFunction<auto(TaskHubProxy) &&->TaskContinuation<Result_T>>;

}   // namespace ddge::scheduler
