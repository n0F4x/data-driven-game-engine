module;

#include <variant>

export module ddge.modules.scheduler.TaskFactory;

import ddge.modules.scheduler.EmbeddedTaskFactory;
import ddge.modules.scheduler.IndirectTaskFactory;
import ddge.modules.scheduler.TaskIndex;

namespace ddge::scheduler {

export template <typename Result_T>
using TaskFactory =
    std::variant<EmbeddedTaskFactory<Result_T>, IndirectTaskFactory<Result_T>>;

}   // namespace ddge::scheduler
