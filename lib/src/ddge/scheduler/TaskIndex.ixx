export module ddge.scheduler.TaskIndex;

import ddge.scheduler.data_structures.WorkIndex;
import ddge.util.containers.Strong;

namespace ddge::scheduler {

export struct TaskIndex : util::Strong<WorkIndex::Underlying, TaskIndex> {
    using Strong::Strong;
};

}   // namespace ddge::scheduler
