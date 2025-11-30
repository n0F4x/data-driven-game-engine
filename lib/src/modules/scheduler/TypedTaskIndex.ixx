export module ddge.modules.scheduler.TypedTaskIndex;

import ddge.modules.scheduler.TaskIndex;

namespace ddge::scheduler {

export template <typename Result_T>
struct TypedTaskIndex : TaskIndex {
    using TaskIndex::TaskIndex;

    [[nodiscard]]
    auto untyped() const -> TaskIndex
    {
        return TaskIndex{ *this };
    }
};

}   // namespace ddge::scheduler
