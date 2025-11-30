export module ddge.modules.exec.TypedTaskIndex;

import ddge.modules.exec.TaskIndex;

namespace ddge::exec {

export template <typename Result_T>
struct TypedTaskIndex : TaskIndex {
    using TaskIndex::TaskIndex;

    [[nodiscard]]
    auto untyped() const -> TaskIndex
    {
        return TaskIndex{ *this };
    }
};

}   // namespace ddge::exec
