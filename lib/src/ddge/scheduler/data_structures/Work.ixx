export module ddge.scheduler.data_structures.Work;

import ddge.scheduler.data_structures.WorkContinuation;

import ddge.util.containers.MoveOnlyFunction;

namespace ddge::scheduler {

export using Work = util::MoveOnlyFunction<WorkContinuation()>;

}   // namespace ddge::scheduler
