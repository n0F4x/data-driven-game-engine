export module ddge.modules.exec.v2.TaskBundle;

import ddge.modules.exec.v2.TaskHubProxy;
import ddge.modules.exec.v2.TaskIndex;

import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::exec::v2 {

using TaskBundleUnderylingFunction = util::AnyMoveOnlyFunction<void(const TaskHubProxy&)>;

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
