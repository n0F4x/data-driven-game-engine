export module ddge.modules.exec.scheduler.Work;

import ddge.modules.exec.scheduler.WorkContinuation;

import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::exec {

export using Work = util::AnyMoveOnlyFunction<WorkContinuation()>;

}   // namespace ddge::exec
