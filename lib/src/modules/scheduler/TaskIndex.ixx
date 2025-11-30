export module ddge.modules.scheduler.TaskIndex;

import ddge.modules.scheduler.data_structures.WorkIndex;
import ddge.utility.containers.Strong;

namespace ddge::scheduler {

export struct TaskIndex : util::Strong<WorkIndex::Underlying, TaskIndex> {
    using Strong::Strong;
};

}   // namespace ddge::scheduler
