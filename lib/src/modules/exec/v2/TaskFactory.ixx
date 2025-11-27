module;

#include <variant>

export module ddge.modules.exec.v2.TaskFactory;

import ddge.modules.exec.v2.EmbeddedTaskFactory;
import ddge.modules.exec.v2.TaskIndex;

namespace ddge::exec::v2 {

export template <typename Result_T>
using TaskFactory = std::variant<EmbeddedTaskFactory<Result_T>>;

}   // namespace ddge::exec::v2
