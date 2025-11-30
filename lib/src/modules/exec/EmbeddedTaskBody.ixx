export module ddge.modules.exec.EmbeddedTaskBody;

import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::exec {

export template <typename Result_T>
using EmbeddedTaskBody = util::AnyMoveOnlyFunction<Result_T()>;

}   // namespace ddge::exec
