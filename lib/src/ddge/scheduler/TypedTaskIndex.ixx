export module ddge.scheduler.TypedTaskIndex;

import ddge.scheduler.TaskIndex;

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
