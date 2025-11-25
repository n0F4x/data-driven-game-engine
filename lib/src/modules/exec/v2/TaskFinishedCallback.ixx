export module ddge.modules.exec.v2.TaskFinishedCallback;

import ddge.modules.exec.v2.TaskHubProxy;

import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::exec::v2 {

namespace {

template <typename Result_T>
struct Impl {
    using type = util::AnyMoveOnlyFunction<void(const TaskHubProxy&, Result_T)>;
};

template <>
struct Impl<void> {
    using type = util::AnyMoveOnlyFunction<void(const TaskHubProxy&)>;
};

}   // namespace

export template <typename Result_T>
using TaskFinishedCallback = Impl<Result_T>::type;

}   // namespace ddge::exec::v2
