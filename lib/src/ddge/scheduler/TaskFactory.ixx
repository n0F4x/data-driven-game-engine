module;

#include <variant>

export module ddge.scheduler.TaskFactory;

import ddge.scheduler.EmbeddedTaskFactory;
import ddge.scheduler.IndirectTaskFactory;
import ddge.scheduler.TaskIndex;

namespace ddge::scheduler {

export template <typename Result_T>
using TaskFactory =
    std::variant<EmbeddedTaskFactory<Result_T>, IndirectTaskFactory<Result_T>>;

}   // namespace ddge::scheduler
