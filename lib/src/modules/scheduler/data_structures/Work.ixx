export module ddge.modules.scheduler.data_structures.Work;

import ddge.modules.scheduler.data_structures.WorkContinuation;

import ddge.util.containers.AnyMoveOnlyFunction;

namespace ddge::scheduler {

export using Work = util::AnyMoveOnlyFunction<WorkContinuation()>;

}   // namespace ddge::scheduler
