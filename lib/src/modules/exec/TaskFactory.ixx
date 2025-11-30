module;

#include <variant>

export module ddge.modules.exec.TaskFactory;

import ddge.modules.exec.EmbeddedTaskFactory;
import ddge.modules.exec.IndirectTaskFactory;
import ddge.modules.exec.TaskIndex;

namespace ddge::exec {

export template <typename Result_T>
using TaskFactory =
    std::variant<EmbeddedTaskFactory<Result_T>, IndirectTaskFactory<Result_T>>;

}   // namespace ddge::exec
