module;

#include <function2/function2.hpp>

export module ddge.modules.execution.scheduler.Work;

import ddge.modules.execution.scheduler.WorkContinuation;

namespace ddge::exec {

export using Work = fu2::unique_function<WorkContinuation()>;

}   // namespace ddge::exec
