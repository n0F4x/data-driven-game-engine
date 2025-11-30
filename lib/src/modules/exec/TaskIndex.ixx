export module ddge.modules.exec.TaskIndex;

import ddge.modules.exec.scheduler.WorkIndex;
import ddge.utility.containers.Strong;

namespace ddge::exec {

export struct TaskIndex : util::Strong<WorkIndex::Underlying, TaskIndex> {
    using Strong::Strong;
};

}   // namespace ddge::exec
