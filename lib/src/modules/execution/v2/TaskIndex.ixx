export module ddge.modules.execution.v2.TaskIndex;

import ddge.modules.execution.scheduler.WorkIndex;
import ddge.utility.containers.Strong;

namespace ddge::exec::v2 {

export struct TaskIndex : util::Strong<WorkIndex::Underlying, TaskIndex> {
    using Strong::Strong;
};

}   // namespace ddge::exec::v2
