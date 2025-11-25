export module ddge.modules.exec.v2.EmbeddedTaskBody;

import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::exec::v2 {

export template <typename Result_T>
using EmbeddedTaskBody = util::AnyMoveOnlyFunction<Result_T()>;

}   // namespace ddge::exec::v2
