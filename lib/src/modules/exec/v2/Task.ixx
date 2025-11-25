export module ddge.modules.exec.v2.Task;

import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::exec::v2 {

export using Task = util::AnyMoveOnlyFunction<void()>;

}   // namespace ddge::exec::v2
