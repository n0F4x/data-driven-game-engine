export module ddge.modules.execution.scheduler.WorkContinuation;

namespace ddge::exec {

export enum struct WorkContinuation {
    eDontCare,
    eReschedule,
    eRelease,
};

}   // namespace ddge::exec
