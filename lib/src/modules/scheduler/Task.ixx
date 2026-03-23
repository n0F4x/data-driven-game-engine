export module ddge.modules.scheduler.Task;

import ddge.util.containers.AnyMoveOnlyFunction;

namespace ddge::scheduler {

export using Task = util::AnyMoveOnlyFunction<void()>;

}   // namespace ddge::scheduler
