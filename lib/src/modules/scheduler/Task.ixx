export module ddge.modules.scheduler.Task;

import ddge.utility.containers.MoveOnlyFunction;

namespace ddge::scheduler {

export using Task = util::MoveOnlyFunction<void()>;

}   // namespace ddge::scheduler
