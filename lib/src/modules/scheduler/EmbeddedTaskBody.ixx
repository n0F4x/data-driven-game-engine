export module ddge.modules.scheduler.EmbeddedTaskBody;

import ddge.utility.containers.MoveOnlyFunction;

namespace ddge::scheduler {

export template <typename Result_T>
using EmbeddedTaskBody = util::MoveOnlyFunction<Result_T()>;

}   // namespace ddge::scheduler
