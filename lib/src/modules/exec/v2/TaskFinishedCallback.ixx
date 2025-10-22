module;

#include <function2/function2.hpp>

export module ddge.modules.exec.v2.TaskFinishedCallback;

import ddge.modules.exec.v2.TaskHubProxy;

namespace ddge::exec::v2 {

namespace {

// TODO: std::move_only_function

template <typename Result_T>
struct Impl {
    using type = fu2::unique_function<void(const TaskHubProxy&, Result_T)>;
};

template <>
struct Impl<void> {
    using type = fu2::unique_function<void(const TaskHubProxy&)>;
};

}   // namespace

export template <typename Result_T>
using TaskFinishedCallback = Impl<Result_T>::type;

}   // namespace ddge::exec::v2
