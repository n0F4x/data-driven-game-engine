module;

#include <function2/function2.hpp>

export module ddge.modules.exec.scheduler.Work;

import ddge.modules.exec.scheduler.WorkContinuation;

namespace ddge::exec {

export using Work = fu2::unique_function<WorkContinuation()>;

}   // namespace ddge::exec
