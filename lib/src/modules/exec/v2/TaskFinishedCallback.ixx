export module ddge.modules.exec.v2.TaskFinishedCallback;

import ddge.modules.exec.v2.TaskHubProxy;

import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::exec::v2 {

template <typename Result_T>
struct TaskFinishedCallbackImpl {
    using type = util::AnyMoveOnlyFunction<void(const TaskHubProxy&, Result_T)>;
};

template <>
struct TaskFinishedCallbackImpl<void> {
    using type = util::AnyMoveOnlyFunction<void(const TaskHubProxy&)>;
};

export template <typename Result_T>
using TaskFinishedCallback = TaskFinishedCallbackImpl<Result_T>::type;

}   // namespace ddge::exec::v2
