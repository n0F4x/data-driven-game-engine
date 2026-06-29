export module ddge.modules.scheduler.data_structures.Work;

import ddge.modules.scheduler.data_structures.WorkContinuation;

import ddge.utility.containers.MoveOnlyFunction;

namespace ddge::scheduler {

export using Work = util::MoveOnlyFunction<WorkContinuation()>;

}   // namespace ddge::scheduler
