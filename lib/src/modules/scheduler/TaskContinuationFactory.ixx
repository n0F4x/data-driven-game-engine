export module ddge.modules.scheduler.TaskContinuationFactory;

import ddge.modules.scheduler.TaskContinuation;
import ddge.modules.scheduler.TaskHubProxy;
import ddge.util.containers.MoveOnlyFunction;

namespace ddge::scheduler {

export template <typename Result_T>
using TaskContinuationFactory =
    util::MoveOnlyFunction<auto(TaskHubProxy) &&->TaskContinuation<Result_T>>;

}   // namespace ddge::scheduler
