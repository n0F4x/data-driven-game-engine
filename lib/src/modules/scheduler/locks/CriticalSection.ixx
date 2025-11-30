module;

#include <variant>

export module ddge.modules.scheduler.locks.CriticalSection;

import ddge.modules.scheduler.locks.ExclusiveCriticalSection;
import ddge.modules.scheduler.locks.SharedCriticalSection;

namespace ddge::scheduler {

export using CriticalSection =
    std::variant<ExclusiveCriticalSection, SharedCriticalSection>;

}   // namespace ddge::scheduler
