export module ddge.modules.exec.Task;

import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::exec {

export template <typename Result_T>
using Task = util::AnyMoveOnlyFunction<Result_T()>;

}   // namespace ddge::exec
