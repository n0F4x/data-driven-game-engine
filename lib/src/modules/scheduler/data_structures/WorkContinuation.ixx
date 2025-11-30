export module ddge.modules.scheduler.data_structures.WorkContinuation;

namespace ddge::scheduler {

export enum struct WorkContinuation {
    eDontCare,
    eReschedule,
    eRelease,
};

}   // namespace ddge::scheduler
