module;

#include <bit>
#include <expected>
#include <limits>

export module ddge.modules.exec.v2.TaskHub;

import ddge.modules.exec.scheduler.WorkTree;
import ddge.modules.exec.v2.Task;
import ddge.modules.exec.v2.TaskIndex;

import ddge.modules.exec.v2.ExecPolicy;

namespace ddge::exec::v2 {

export class TaskHub {
public:
    constexpr static TaskIndex::Underlying task_index_value_mask =
        std::numeric_limits<TaskIndex::Underlying>::max() >> 2;

    constexpr static TaskIndex::Underlying task_index_tag_mask =
        std::numeric_limits<TaskIndex::Underlying>::max() - task_index_value_mask;

    struct IndexTags {
        constexpr static TaskIndex::Underlying generic = 0;
        constexpr static TaskIndex::Underlying main_only =
            std::numeric_limits<TaskIndex::Underlying>::max()
            - (std::numeric_limits<TaskIndex::Underlying>::max() >> 1);
    };

    explicit TaskHub(
        uint64_t generic_capacity,
        uint64_t main_only_capacity,
        uint32_t number_of_threads
    );

    [[nodiscard]]
    auto try_emplace_generic_at(Task&& task, TaskIndex task_index)
        -> std::expected<void, Task>;
    [[nodiscard]]
    auto try_emplace_main_only_at(Task&& task, TaskIndex task_index)
        -> std::expected<void, Task>;

    auto schedule(TaskIndex task_index) -> void;

    auto try_execute_a_generic_task(uint32_t thread_id) -> bool;
    auto try_execute_a_main_only_task() -> bool;

private:
    WorkTree m_generic_work_tree;
    WorkTree m_main_only_work_tree;
};

}   // namespace ddge::exec::v2
