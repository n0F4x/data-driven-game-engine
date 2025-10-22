export module ddge.modules.exec.scheduler.WorkContinuation;

namespace ddge::exec {

export enum struct WorkContinuation {
    eDontCare,
    eReschedule,
    eRelease,
};

}   // namespace ddge::exec
