export module ddge.modules.scheduler.EmbeddedTaskBody;

import ddge.util.containers.MoveOnlyFunction;

namespace ddge::scheduler {

export template <typename Result_T>
using EmbeddedTaskBody = util::MoveOnlyFunction<Result_T()>;

}   // namespace ddge::scheduler
