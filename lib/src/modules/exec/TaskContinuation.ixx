export module ddge.modules.exec.TaskContinuation;

import ddge.utility.containers.AnyMoveOnlyFunction;
import ddge.utility.meta.concepts.decayed;

namespace ddge::exec {

template <typename Result_T>
struct TaskContinuationImpl {
    using type = util::AnyMoveOnlyFunction<void(Result_T&&)>;
};

template <>
struct TaskContinuationImpl<void> {
    using type = util::AnyMoveOnlyFunction<void()>;
};

export template <util::meta::decayed_c Result_T>
using TaskContinuation = TaskContinuationImpl<Result_T>::type;

}   // namespace ddge::exec
