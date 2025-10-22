module;

#include <function2/function2.hpp>

export module ddge.modules.execution.v2.TaskBundle;

import ddge.modules.execution.v2.TaskHubProxy;
import ddge.modules.execution.v2.TaskIndex;

namespace ddge::exec::v2 {

using TaskBundleUnderylingFunction = fu2::unique_function<void(const TaskHubProxy&)>;

export class TaskBundle : public TaskBundleUnderylingFunction {
public:
    using TaskBundleUnderylingFunction::TaskBundleUnderylingFunction;

    explicit TaskBundle(const TaskIndex task_index)
        : TaskBundleUnderylingFunction{
              [task_index](const TaskHubProxy& task_hub_proxy) -> void {
                  task_hub_proxy.schedule(task_index);
              }   //
          }
    {}
};

}   // namespace ddge::exec::v2
