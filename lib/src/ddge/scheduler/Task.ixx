export module ddge.scheduler.Task;

import ddge.util.containers.MoveOnlyFunction;

namespace ddge::scheduler {

export using Task = util::MoveOnlyFunction<void()>;

}   // namespace ddge::scheduler
