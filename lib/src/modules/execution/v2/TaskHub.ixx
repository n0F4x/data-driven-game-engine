module;

#include <bit>
#include <expected>
#include <limits>

export module ddge.modules.execution.v2.TaskHub;

import ddge.modules.execution.scheduler.Work;
import ddge.modules.execution.scheduler.WorkIndex;
import ddge.modules.execution.scheduler.WorkTree;

import ddge.modules.execution.v2.ExecPolicy;

namespace ddge::exec::v2 {

export class TaskHub {
public:
    constexpr static WorkIndex::Underlying work_index_tag_mask =
        std::numeric_limits<WorkIndex::Underlying>::max()
        - (std::numeric_limits<WorkIndex::Underlying>::max() >> 2);

    struct IndexTags {
        constexpr static WorkIndex::Underlying generic = 0;
        constexpr static WorkIndex::Underlying main_only =
            std::numeric_limits<WorkIndex::Underlying>::max()
            - (std::numeric_limits<WorkIndex::Underlying>::max() >> 1);
    };

    explicit TaskHub(
        uint64_t generic_capacity,
        uint64_t main_only_capacity,
        uint32_t number_of_threads
    );

    [[nodiscard]]
    auto try_emplace_generic_at(Work&& work, WorkIndex work_index)
        -> std::expected<void, Work>;
    [[nodiscard]]
    auto try_emplace_main_only_at(Work&& work, WorkIndex work_index)
        -> std::expected<void, Work>;

    auto schedule(WorkIndex work_index) -> void;

    auto try_execute_one(uint32_t thread_id) -> bool;
    auto try_execute_one_main_only_work() -> bool;

private:
    WorkTree m_generic_work_tree;
    WorkTree m_main_only_work_tree;
};

}   // namespace ddge::exec::v2
