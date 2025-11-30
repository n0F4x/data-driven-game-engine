export module ddge.modules.exec.Task;

import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::exec {

export using Task = util::AnyMoveOnlyFunction<void()>;

}   // namespace ddge::exec
