export module ddge.modules.exec.v2.TaskIndex;

import ddge.modules.exec.scheduler.WorkIndex;
import ddge.utility.containers.Strong;

namespace ddge::exec::v2 {

export struct TaskIndex : util::Strong<WorkIndex::Underlying, TaskIndex> {
    using Strong::Strong;
};

}   // namespace ddge::exec::v2
