export module ddge.modules.scheduler.EmbeddedTaskBody;

import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::scheduler {

export template <typename Result_T>
using EmbeddedTaskBody = util::AnyMoveOnlyFunction<Result_T()>;

}   // namespace ddge::scheduler
