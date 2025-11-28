export module ddge.modules.exec.v2.IndirectTaskBody;

import ddge.modules.exec.v2.TaskHubProxy;

import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::exec::v2 {

export using IndirectTaskBody = util::AnyMoveOnlyFunction<void(const TaskHubProxy&)>;

}   // namespace ddge::exec::v2
