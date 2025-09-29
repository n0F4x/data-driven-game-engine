export module ddge.modules.execution.v2.ScheduleBuilder;

import ddge.modules.execution.v2.TaskBuilder;
import ddge.modules.execution.v2.TaskFinishedCallback;

namespace ddge::exec::v2 {

export class ScheduleBuilder {
public:
    explicit ScheduleBuilder(TaskBuilder<void>&& initial);

    [[nodiscard]]
    auto then(TaskBuilder<void>&& next) && -> ScheduleBuilder;

    explicit(false) operator TaskBuilder<void>() &&;

private:
    TaskBuilder<void> m_task_builder;
};

}   // namespace ddge::exec::v2
