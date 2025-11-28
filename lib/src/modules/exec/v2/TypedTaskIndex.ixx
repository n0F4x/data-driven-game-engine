export module ddge.modules.exec.v2.TypedTaskIndex;

import ddge.modules.exec.v2.TaskIndex;

namespace ddge::exec::v2 {

export template <typename Result_T>
struct TypedTaskIndex : TaskIndex {
    using TaskIndex::TaskIndex;
};

}   // namespace ddge::exec::v2
